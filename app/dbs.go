package app

func (a *App) getdb(dsn string) DBI {
	if db, ok := a.dbs[dsn]; ok {
		return db
	}
	db := DBPg_New(dsn)
	a.dbs[dsn] = db
	return db
}

func (a *App) Dbs_GetSchema(dsn string, force bool) (Schema, error) {
	db := a.getdb(dsn)
	return db.GetSchema()
}

func (a *App) Dbs_Query(dsn string, qry string) Result {
	db := a.getdb(dsn)
	return db.Exec(qry)
}
