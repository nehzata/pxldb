import * as path from 'path';
import {defineConfig} from 'vite'
import react from '@vitejs/plugin-react'
import svgr from 'vite-plugin-svgr';

// https://vitejs.dev/config/
export default defineConfig({
  resolve: {
    alias: {
      '@pxl': path.resolve('src'),
      '@wails': path.resolve('wailsjs'),
    },
  },
  plugins: [svgr({include: '**/*.svg'}), react()]
})
