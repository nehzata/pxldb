.SECONDEXPANSION:

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

.PHONY :regen
regen: Makefile db.go query.sql.go models.go ## Rebuild the sqlc database access layers

db.go query.sql.go models.go: Makefile sqlc.yaml queries/*.sql schema/*.sql
	@sqlc generate