-- +goose Up

ALTER TABLE session_queries ADD COLUMN expln TEXT;
ALTER TABLE session_queries ADD COLUMN anlyz TEXT;