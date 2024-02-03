-- name: KvStoreGet :one
SELECT v
FROM kvstore
WHERE k = @key;

-- name: KvStoreSet :exec
INSERT INTO kvstore (k, v)
VALUES (@key, @val)
ON CONFLICT (k)
DO UPDATE
  SET v = EXCLUDED.v;

-- name: DbsGet :many
SELECT id, dsn, name, is_expanded
FROM dbs
WHERE delete_ts IS NULL;

-- name: DbsInsert :one
INSERT INTO dbs (dsn, name)
VALUES (@dsn, @name)
RETURNING id;

-- name: DbsUpdate :exec
UPDATE dbs
SET dsn = @dsn, name = @name
WHERE id = @id;

-- name: DbsDelete :exec
UPDATE dbs
SET delete_ts = UNIXEPOCH()
WHERE id = @id;

-- name: DbsSetIsExpanded :exec
UPDATE dbs
SET is_expanded = @is_expanded
WHERE id = @id;

-- name: SessionsGet :many
SELECT id, db_id, title
FROM sessions
WHERE delete_ts IS NULL;

-- name: SessionsInsert :one
INSERT INTO sessions (id, db_id)
VALUES (@id, @db_id)
RETURNING id;

-- name: SessionsDelete :exec
UPDATE sessions
SET delete_ts = UNIXEPOCH()
WHERE id = @id;

-- name: SessionsUpdate :exec
UPDATE sessions
SET title = @title
WHERE id = @id;

-- name: SessionQueriesGet :many
SELECT id, qry, res
FROM session_queries
WHERE
  session_id = @s_id AND
  delete_ts IS NULL
ORDER BY create_ts;

-- name: SessionQueriesInsert :one
INSERT INTO session_queries (session_id, qry, res)
VALUES (@s_id, @qry, @res)
RETURNING id;

-- name: SessionQueriesUpdate :exec
UPDATE session_queries
SET qry = @qry, res = @res
WHERE id = @id;

-- name: SessionQueriesDelete :exec
UPDATE session_queries
SET delete_ts = UNIXEPOCH()
WHERE id = @id;