package app

type Column struct {
	Name      string `json:"name"`
	Desc      string `json:"desc"`
	ShortDesc string `json:"shortDesc"`
}
type Table struct {
	Name string   `json:"name"`
	Cols []Column `json:"cols"`
}
type Schema struct {
	Tables []Table `json:"tables"`
}

type Result struct {
	Columns []string   `json:"columns"`
	Rows    [][]string `json:"rows"`
	NumRows int        `json:"numRows"`
	Error   *string    `json:"error"`
}

type DBI interface {
	GetSchema() (Schema, error)
	Exec(qry string) Result
}
