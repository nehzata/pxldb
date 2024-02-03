# README

## About

This is the official Wails React template.

You can configure the project by editing `wails.json`. More information about the project settings can be found
here: https://wails.io/docs/reference/project-config

## Getting started

```
. bin/activate-hermit
go install github.com/wailsapp/wails/v2/cmd/wails@latest
git submodule init
git submodule update --remote
cd frontend/pg-explain && npm i && npm run build-pxl
```

## Live Development

To run in live development mode, run `wails dev` in the project directory. This will run a Vite development
server that will provide very fast hot reload of your frontend changes. If you want to develop in a browser
and have access to your Go methods, there is also a dev server that runs on http://localhost:34115. Connect
to this in your browser, and you can call your Go code from devtools.

## Building

To build a redistributable, production mode package, use `wails build`.

## TODO
- <s>rename sessions</s>
- fuzzy search
- <s>autocomplete integration</s>
- grammar updates
  - <s>case insensitive</s>
  - update grammar
  - insert grammar
  - delete grammar
- <s>download model on first run</s>
- plan / explain view improvements