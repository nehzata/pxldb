import React from 'react';
import ReactDOM from 'react-dom/client';
import * as Wails from '@wails/go/app/App.js';
import * as Runtime from '@wails/runtime';
// import './style.css'
// import App from './App'

import App from './components/App';

import reducers from './reducers';
import StoreProvider from './utils/store-provider.jsx';

ReactDOM.createRoot(document.getElementById('root')).render(
    <StoreProvider reducers={reducers}>
        <App />
    </StoreProvider>
);

// const container = document.getElementById('root')

// const root = createRoot(container)

// root.render(
//     <React.StrictMode>
//         <App/>
//     </React.StrictMode>
// )
