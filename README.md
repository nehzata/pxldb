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

## Dist

```
security find-identity -v -p codesigning | grep Developer
xcrun notarytool store-credentials "notarytool-password" --apple-id <APPLE_ID_EMAIL> --team-id <TEAM_ID> --password <APPLE_ID_APP_PASSWORD>
```

## TODO
- <s>rename sessions</s>
- fuzzy search
- anonymous usage tracking
- <s>autocomplete integration</s>
- grammar updates
  - <s>case insensitive</s>
  - type casts
  - range & between expressions
  - function expressions
  - update grammar
  - insert grammar
  - delete grammar
- <s>download model on first run</s>
- plan / explain view improvements
  - failed query plan causes error