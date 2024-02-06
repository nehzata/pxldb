package app

import (
	"context"
	"database/sql"
	"encoding/json"
	"errors"
	"math"
	"strings"

	"github.com/gofrs/uuid/v5"
	_ "github.com/mattn/go-sqlite3"
	"github.com/thnxdev/pxldb/database"
)

type WinBounds struct {
	X int `json:"x"`
	Y int `json:"y"`
	W int `json:"w"`
	H int `json:"h"`
}

func (a *App) Cfg_WinBoundsGet() (WinBounds, error) {
	_winBounds, err := a.cfg.KvStoreGet(context.Background(), "win-bounds")
	if err != nil {
		if errors.Is(err, sql.ErrNoRows) {
			return WinBounds{200, 200, 1024, 768}, nil
		}
		return WinBounds{}, err
	}

	var res WinBounds
	err = json.Unmarshal([]byte(_winBounds), &res)
	if err != nil {
		return WinBounds{}, err
	}
	res.X = int(math.Max(float64(res.X), 20))
	res.Y = int(math.Max(float64(res.Y), 20))
	if res.W <= 100 {
		res.W = 1024
	}
	if res.H < 100 {
		res.H = 768
	}

	return res, nil
}

func (a *App) Cfg_WinBoundsSet(wb WinBounds) error {
	wbStr, err := json.Marshal(wb)
	if err != nil {
		return err
	}

	err = a.cfg.KvStoreSet(context.Background(), database.KvStoreSetParams{
		"win-bounds",
		string(wbStr),
	})
	if err != nil {
		return err
	}

	return nil
}

func (a *App) Cfg_LlmEnabledGet() (bool, error) {
	_enabled, err := a.cfg.KvStoreGet(context.Background(), "llm-enabled")
	if err != nil {
		if errors.Is(err, sql.ErrNoRows) {
			return true, nil
		}
		return false, err
	}
	var res bool
	err = json.Unmarshal([]byte(_enabled), &res)
	if err != nil {
		return false, err
	}
	return res, nil
}

func (a *App) Cfg_LlmEnabledSet(enabled bool) error {
	enabledStr, err := json.Marshal(enabled)
	if err != nil {
		return err
	}

	err = a.cfg.KvStoreSet(context.Background(), database.KvStoreSetParams{
		"llm-enabled",
		string(enabledStr),
	})
	if err != nil {
		return err
	}

	return nil
}

type DB struct {
	ID         int64  `json:"id"`
	Dsn        string `json:"dsn"`
	Name       string `json:"name"`
	IsExpanded bool   `json:"isExpanded"`
}

func (a *App) Cfg_DbsGet() []DB {
	_dbs, err := a.cfg.DbsGet(context.Background())
	if err != nil {
		return []DB{}
	}

	dbs := []DB{}
	for _, db := range _dbs {
		dbs = append(dbs, DB{
			ID:         db.ID,
			Dsn:        db.Dsn,
			Name:       db.Name,
			IsExpanded: db.IsExpanded,
		})
	}
	return dbs
}

func (a *App) Cfg_DbsInsert(dsn string, name string) int64 {
	id, _ := a.cfg.DbsInsert(context.Background(), database.DbsInsertParams{
		Dsn:  dsn,
		Name: name,
	})
	return id
}

func (a *App) Cfg_DbsUpdate(id int64, dsn string, name string) {
	a.cfg.DbsUpdate(context.Background(), database.DbsUpdateParams{
		ID:   id,
		Dsn:  dsn,
		Name: name,
	})
}

func (a *App) Cfg_DbsDelete(id int64) {
	a.cfg.DbsDelete(context.Background(), id)
}

func (a *App) Cfg_DbsSetIsExpanded(id int64, isExpanded bool) {
	a.cfg.DbsSetIsExpanded(context.Background(), database.DbsSetIsExpandedParams{
		ID:         id,
		IsExpanded: isExpanded,
	})
}

type Session struct {
	ID    string  `json:"id"`
	DbID  int64   `json:"dbId"`
	Title *string `json:"title"`
}

func (a *App) Cfg_SessionsGet() []Session {
	_sessions, err := a.cfg.SessionsGet(context.Background())
	if err != nil {
		return []Session{}
	}

	sessions := []Session{}
	for _, s := range _sessions {
		s := s
		sess := Session{
			ID:   s.ID,
			DbID: s.DbID,
		}
		if s.Title.Valid {
			sess.Title = &s.Title.String
		}
		sessions = append(sessions, sess)
	}
	return sessions
}

func (a *App) Cfg_SessionsInsert(dbID int64) string {
	_sid, _ := uuid.NewV4()
	sid := _sid.String()
	id, _ := a.cfg.SessionsInsert(context.Background(), database.SessionsInsertParams{
		ID:   sid,
		DbID: dbID,
	})
	return id
}

func (a *App) Cfg_SessionsUpdate(sID string, title string) {
	var t sql.NullString
	titleTrimmed := strings.TrimSpace(title)
	if titleTrimmed != "" {
		t = sql.NullString{titleTrimmed, true}
	}
	a.cfg.SessionsUpdate(context.Background(), database.SessionsUpdateParams{
		ID:    sID,
		Title: t,
	})
}

func (a *App) Cfg_SessionsDelete(sID string) {
	a.cfg.SessionsDelete(context.Background(), sID)
}

type SessionQuery struct {
	ID  int64  `json:"id"`
	Qry string `json:"qry"`
	Res any    `json:"res"`
}

func (a *App) Cfg_GetSessionQueries(sID string) []SessionQuery {
	_queries, err := a.cfg.SessionQueriesGet(context.Background(), sID)
	if err != nil {
		return []SessionQuery{}
	}

	queries := []SessionQuery{}
	for _, q := range _queries {
		q := q
		tmp := SessionQuery{
			ID:  q.ID,
			Qry: q.Qry,
		}
		if q.Res.Valid {
			json.Unmarshal([]byte(q.Res.String), &tmp.Res)
			// tmp.Res = &q.Res.String
		}
		queries = append(queries, tmp)
	}
	return queries
}

func (a *App) Cfg_SessionQueriesInsert(sID string, qry string, res string) int64 {
	id, _ := a.cfg.SessionQueriesInsert(context.Background(), database.SessionQueriesInsertParams{
		SID: sID,
		Qry: qry,
		Res: sql.NullString{res, true},
	})
	return id
}

func (a *App) Cfg_SessionQueriesUpdate(id int64, qry string, res string) {
	a.cfg.SessionQueriesUpdate(context.Background(), database.SessionQueriesUpdateParams{
		ID:  id,
		Qry: qry,
		Res: sql.NullString{res, true},
	})
}

func (a *App) Cfg_SessionQueriesDelete(id int64) {
	a.cfg.SessionQueriesDelete(context.Background(), id)
}
