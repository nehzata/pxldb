package app

import (
	"encoding/csv"
	"os"

	wails "github.com/wailsapp/wails/v2/pkg/runtime"
)

func (a *App) Utils_ExportRows(fn string, records [][]string) error {
	filename, err := wails.SaveFileDialog(a.ctx, wails.SaveDialogOptions{
		DefaultFilename: fn,
		Title:           "Select file",
		Filters: []wails.FileFilter{
			{
				DisplayName: "CSV (*.csv)",
				Pattern:     "*.csv",
			},
		},
	})
	if err != nil {
		return err
	} else if filename == "" {
		return nil
	}

	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	w := csv.NewWriter(f)

	for _, record := range records {
		if err := w.Write(record); err != nil {
			return err
		}
	}

	// Write any buffered data to the underlying writer
	w.Flush()

	if err := w.Error(); err != nil {
		return err
	}

	return nil
}
