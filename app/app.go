package app

import (
	"context"
	"database/sql"
	_ "embed"
	"fmt"
	"os"

	"github.com/adrg/xdg"
	"github.com/alecthomas/errors"
	_ "github.com/mattn/go-sqlite3"
	"github.com/thnxdev/pxldb/database"
	wails "github.com/wailsapp/wails/v2/pkg/runtime"
)

type ModelState int

const (
	MODEL_STATE_IDLE ModelState = iota
	MODEL_STATE_DOWNLOADING
	MODEL_STATE_LOADING
	MODEL_STATE_RUNNING
)

// App struct
type App struct {
	ctx context.Context
	cfg *database.Queries
	dbs map[string]DBI

	modelState ModelState
}

// NewApp creates a new App application struct
func NewApp() *App {
	err := os.MkdirAll(fmt.Sprintf("%s/PXLDB", xdg.ConfigHome), os.ModePerm)
	if err != nil {
		panic(errors.Wrap(err, "failed to create config home path"))
	}

	cfgPath, err := xdg.ConfigFile("PXLDB/db.sql")
	if err != nil {
		panic(err)
	}

	// Run migrations
	err = database.Migrate(context.Background(), cfgPath)
	if err != nil {
		panic(err)
	}

	// Open connection to db
	conn, err := sql.Open("sqlite3", cfgPath)
	if err != nil {
		panic(errors.Wrap(err, "failed to open db"))
	}

	return &App{
		cfg:        database.New(conn),
		dbs:        map[string]DBI{},
		modelState: MODEL_STATE_IDLE,
	}
}

// startup is called when the app starts. The context is saved
// so we can call the runtime methods
func (a *App) Startup(ctx context.Context) {
	a.ctx = ctx
}

func (a *App) OnDomReady(ctx context.Context) {
	wb, err := a.Cfg_WinBoundsGet()
	if err == nil {
		wails.WindowSetPosition(a.ctx, wb.X, wb.Y)
		wails.WindowSetSize(a.ctx, wb.W, wb.H)
	}
	wails.WindowShow(a.ctx)
}

func (a *App) Shutdown(ctx context.Context) {
	x, y := wails.WindowGetPosition(a.ctx)
	w, h := wails.WindowGetSize(a.ctx)
	wb := WinBounds{x, y, w, h}
	a.Cfg_WinBoundsSet(wb)
}
