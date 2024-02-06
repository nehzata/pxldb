import React from 'react';
import ReactDOM from 'react-dom/client';

import App from './components/App';
import Llm from './components/Llm';

import reducers from './reducers';
import StoreProvider from './utils/store-provider.jsx';

ReactDOM.createRoot(document.getElementById('root')).render(
    <StoreProvider reducers={reducers}>
        <Llm>
            <App />
        </Llm>
    </StoreProvider>
);