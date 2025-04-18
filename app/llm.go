package app

// CGO tutorials
// https://karthikkaranth.me/blog/calling-c-code-from-go/
// https://eli.thegreenplace.net/2019/passing-callbacks-and-pointers-to-cgo/

// #cgo CXXFLAGS: -g -Wall -std=c++14 -fPIC
// #cgo CXXFLAGS: -I ../llm/src
// #cgo CXXFLAGS: -DFMT_HEADER_ONLY -I ../llm/third_party/fmt/include
// #cgo CXXFLAGS: -I ../llm/third_party/llama.cpp
// #cgo CXXFLAGS: -DSPDLOG_FMT_EXTERNAL -I ../llm/third_party/spdlog/include
// #cgo LDFLAGS: -L ../llm -lllm
// #cgo LDFLAGS: -L ../llm/third_party/llama.cpp -lllama
// #cgo LDFLAGS: -framework Foundation -framework Metal
// #cgo LDFLAGS: -framework MetalKit -framework Accelerate
// #include <stdlib.h>
// #include "llm.h"
import "C"

import (
	"fmt"
	"os"
	"time"
	"unsafe"

	"github.com/adrg/xdg"
	"github.com/cavaliergopher/grab/v3"
	gopointer "github.com/mattn/go-pointer"
	wails "github.com/wailsapp/wails/v2/pkg/runtime"
)

var MODELS map[string]string = map[string]string{
	"phi-2":                    "https://huggingface.co/nakcnx/phi-2-sql-gguf/resolve/main/phi-2-sql-Q5_K_M.gguf?download=true",
	"phi-2-sql":                "https://huggingface.co/nakcnx/phi-2-sql-gguf/resolve/main/phi-2-sql-Q5_K_M.gguf?download=true",
	"sqlcoder-7b":              "https://huggingface.co/TheBloke/sqlcoder-7B-GGUF/resolve/main/sqlcoder-7b.Q4_0.gguf?download=true",
	"mistral-7b":               "https://huggingface.co/TheBloke/Mistral-7B-v0.1-GGUF/resolve/main/mistral-7b-v0.1.Q4_0.gguf?download=true",
	"mistral-7b-instruct":      "https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.2-GGUF/resolve/main/mistral-7b-instruct-v0.2.Q4_0.gguf?download=true",
	"mistral-7b-instruct-code": "https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.2-code-ft-GGUF/resolve/main/mistral-7b-instruct-v0.2-code-ft.Q4_0.gguf?download=true",
	"deepseek-llm-7b-base":     "https://huggingface.co/TheBloke/deepseek-llm-7B-base-GGUF/resolve/main/deepseek-llm-7b-base.Q4_0.gguf?download=true",
	"deepseek-coder-6.7b-base": "https://huggingface.co/TheBloke/deepseek-coder-6.7B-base-GGUF/resolve/main/deepseek-coder-6.7b-base.Q4_0.gguf?download=true",
}
var DEFAULT_MODEL = "mistral-7b-instruct"

func (a *App) llm_init() {
	a.modelState = MODEL_STATE_DOWNLOADING

	modelPath, err := xdg.ConfigFile(fmt.Sprintf("PXLDB/%s", DEFAULT_MODEL))
	if err != nil {
		panic(err)
	}

	url, ok := MODELS[DEFAULT_MODEL]
	if !ok {
		panic(fmt.Errorf("unable to find %s", DEFAULT_MODEL))
	}

	client := grab.NewClient()
	req, _ := grab.NewRequest(modelPath, url)
	resp := client.Do(req)

	t := time.NewTicker(500 * time.Millisecond)

Loop:
	for {
		select {
		case <-t.C:
			wails.EventsEmit(
				a.ctx,
				"llm-status",
				fmt.Sprintf(
					`{"progress": %.2f, "received": %v, "total": %v}`,
					100*resp.Progress(),
					resp.BytesComplete(),
					resp.Size(),
				),
			)

		case <-resp.Done:
			// download is complete
			t.Stop()
			break Loop
		}
	}

	// check for errors
	if err := resp.Err(); err != nil {
		fmt.Fprintf(os.Stderr, "Download failed: %v\n", err)
		wails.EventsEmit(a.ctx, "llm-status", `{"error": true, "code": "ERROR_DOWNLOAD_FAILED"}`)
		return
	}

	a.modelState = MODEL_STATE_LOADING

	mp := C.CString(modelPath)
	defer C.free(unsafe.Pointer(mp))
	res := C.llm_init(mp)
	if res != 0 {
		wails.EventsEmit(a.ctx, "llm-status", `{"error": true, "code": "ERROR_MODEL_LOAD_FAILED"}`)
		return
	}

	C.llm_run()

	// prime the model to load all the weights into the GPU
	p := C.CString("<s></s>")
	defer C.free(unsafe.Pointer(p))

	C.llm_set_schema(C.int(0), p)

	wails.EventsEmit(a.ctx, "llm-status", `{"loaded": true}`)

	a.modelState = MODEL_STATE_RUNNING
}

func (a *App) llm_close() {
	C.llm_close()
	a.modelState = MODEL_STATE_IDLE
	wails.EventsEmit(a.ctx, "llm-status", `{"loaded": false}`)
}

func (a *App) Llm_StatusGet() bool {
	return a.modelState == MODEL_STATE_RUNNING
}

func (a *App) Llm_Init() {
	if a.modelState != MODEL_STATE_IDLE {
		if a.modelState == MODEL_STATE_RUNNING {
			wails.EventsEmit(a.ctx, "llm-status", `{"loaded": true}`)
		}
		return
	}
	go a.llm_init()
}

func (a *App) Llm_Close() {
	if a.modelState != MODEL_STATE_RUNNING {
		return
	}
	a.modelState = MODEL_STATE_CLOSING
	go a.llm_close()
}

func (a *App) Llm_SetSchema(schemaId uint, schema string) {
	if a.modelState != MODEL_STATE_RUNNING {
		return
	}
	s := C.CString(schema)
	defer C.free(unsafe.Pointer(s))

	C.llm_set_schema(C.int(schemaId), s)
}

func (a *App) Llm_Prime(schemaId uint, prompt string) error {
	if a.modelState != MODEL_STATE_RUNNING {
		return nil
	}
	p := C.CString(prompt)
	defer C.free(unsafe.Pointer(p))

	r := C.struct_llm_ctx{
		error_code: (*C.char)(C.malloc(C.sizeof_char * 32)),
	}
	defer C.free(unsafe.Pointer(r.error_code))

	C.llm_prime(C.int(schemaId), p, &r)
	if r.error == 1 {
		return fmt.Errorf("prime failed with %s", C.GoString(r.error_code))
	}
	return nil
	// if r.error == 1 {
	// 	// return LlmResult{
	// 	// 	Error:     true,
	// 	// 	ErrorCode: C.GoString(r.error_code),
	// 	// }
	// } else if r.cancel == 1 {
	// 	// return LlmResult{
	// 	// 	Cancel: true,
	// 	// }
	// }
	// return LlmResult{}
}

func (a *App) Llm_Autocomplete(eventId string, reqId uint, schemaId uint, prompt string) {
	if a.modelState != MODEL_STATE_RUNNING {
		return
	}
	aPtr := gopointer.Save(a)
	defer gopointer.Unref(aPtr)

	eid := C.CString(eventId)
	defer C.free(unsafe.Pointer(eid))

	p := C.CString(prompt)
	defer C.free(unsafe.Pointer(p))

	r := C.struct_llm_ctx{
		app:        aPtr,
		event_id:   eid,
		req_id:     C.int(reqId),
		error:      0,
		error_code: (*C.char)(C.malloc(C.sizeof_char * 32)),
		cancel:     0,
		stop:       0,
		token:      (*C.char)(C.malloc(C.sizeof_char * 256)),
	}
	defer C.free(unsafe.Pointer(r.error_code))
	defer C.free(unsafe.Pointer(r.token))

	cb := C.llm_autocomplete_callback_fn(C.llm_autocomplete_callback)

	C.llm_autocomplete(C.int(schemaId), p, cb, &r)
}

//export go_llm_autocomplete_callback
func go_llm_autocomplete_callback(result *C.struct_llm_ctx) {
	a := gopointer.Restore(result.app).(*App)

	eventId := C.GoString(result.event_id)

	wails.EventsEmit(a.ctx, eventId, map[string]any{
		"reqId":  result.req_id,
		"error":  result.error == 1,
		"cancel": result.cancel == 1,
		"stop":   result.stop == 1,
		"t":      C.GoString(result.token),
	})
}
