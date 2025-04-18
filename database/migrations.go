package database

import (
	"context"
	"database/sql"
	"embed"

	"github.com/alecthomas/errors"
	"github.com/pressly/goose/v3"
)

//go:embed all:schema
var Migrations embed.FS

// Migrate a database connection to the latest schema using Goose.
func Migrate(ctx context.Context, dsn string) error {
	conn, err := sql.Open("sqlite3", dsn)
	if err != nil {
		return errors.Wrap(err, "failed to connect to database")
	}
	defer conn.Close()

	goose.SetDialect("sqlite3")
	goose.SetBaseFS(Migrations)
	goose.SetLogger(goose.NopLogger())
	err = goose.Up(conn, "schema")
	if err != nil {
		return errors.Wrap(err, "failed to run migrations")
	}
	return nil
}
