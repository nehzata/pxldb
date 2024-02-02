package app

import (
	"context"
	"encoding/hex"
	"fmt"
	"strings"
	"time"

	uuid "github.com/gofrs/uuid/v5"
	pgxuuid "github.com/jackc/pgx-gofrs-uuid"
	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgxpool"
)

type DBPg struct {
	*pgxpool.Pool
}

func DBPg_New(dsn string) *DBPg {
	cfg, _ := pgxpool.ParseConfig(dsn)
	cfg.AfterConnect = func(ctx context.Context, c *pgx.Conn) error {
		pgxuuid.Register(c.TypeMap())
		return nil
	}

	pool, _ := pgxpool.NewWithConfig(context.Background(), cfg)
	return &DBPg{pool}
}

func (pg *DBPg) GetSchema() (Schema, error) {
	r1, err := pg.Query(
		context.Background(),
		`
			SELECT table_name
			FROM information_schema.tables
			WHERE table_schema = 'public' AND table_type = 'BASE TABLE'
			ORDER BY table_name
		`,
	)
	if err != nil {
		return Schema{}, err
	}
	defer r1.Close()

	tables, err := pgx.CollectRows(r1, func(row pgx.CollectableRow) (Table, error) {
		var t string
		err := row.Scan(&t)
		if err != nil {
			return Table{}, err
		}

		r2, err := pg.Query(
			context.Background(),
			`
				SELECT column_name, data_type, udt_name, is_nullable, column_default
				FROM information_schema.columns
				WHERE table_name = $1
				ORDER BY ordinal_position
			`,
			t,
		)
		if err != nil {
			return Table{}, err
		}
		defer r2.Close()

		cols, err := pgx.CollectRows(r2, func(row pgx.CollectableRow) (Column, error) {
			var name, ctype, dtname, isnullable string
			var cdefault *string
			err := row.Scan(&name, &ctype, &dtname, &isnullable, &cdefault)
			if err != nil {
				return Column{}, err
			}
			col := Column{
				Name:      name,
				ShortDesc: fmt.Sprintf("%s %s", name, dtname),
			}
			desc := []string{name, dtname}
			if isnullable == "NO" {
				desc = append(desc, "NOT NULL")
			}
			if cdefault != nil {
				desc = append(desc, fmt.Sprintf("DEFAULT %s", *cdefault))
			}
			col.Desc = strings.Join(desc, " ")

			return col, nil
		})

		if err != nil {
			return Table{}, err
		}

		return Table{Name: t, Cols: cols}, nil
	})

	if err != nil {
		return Schema{}, err
	}

	return Schema{Tables: tables}, nil
}

func (pg *DBPg) Exec(qry string) Result {
	var res Result = Result{
		NumRows: 0,
		Columns: []string{},
		Rows:    [][]string{},
		Error:   nil,
	}

	r, err := pg.Query(context.Background(), qry)
	if err != nil {
		errStr := err.Error()
		res.Error = &errStr
		return res
	}

	res.Columns = []string{}

	_cols := r.FieldDescriptions()
	for _, c := range _cols {
		res.Columns = append(res.Columns, c.Name)
	}

	res.Rows, err = pgx.CollectRows(r, func(row pgx.CollectableRow) ([]string, error) {
		vals, err := row.Values()
		if err != nil {
			return []string{}, err
		}

		var ret []string
		for _, _v := range vals {
			var str string
			switch v := _v.(type) {
			case string:
				str = v
			case nil:
				str = "NULL"
			case bool:
				if v {
					str = "T"
				} else {
					str = "F"
				}
			case int, int8, int16, int32, int64:
				str = fmt.Sprintf("%d", v)
			case []byte:
				str = hex.EncodeToString(v)
			case uuid.UUID:
				str = v.String()
			case time.Time:
				str = v.String()
			default:
				str = fmt.Sprintf("unknown type %T", _v)
			}
			ret = append(ret, str)
		}
		return ret, nil
	})

	r.Close()

	if err != nil {
		errStr := err.Error()
		res.Error = &errStr
		return res
	}

	cmd := r.CommandTag()
	res.NumRows = int(cmd.RowsAffected())

	return res
}
