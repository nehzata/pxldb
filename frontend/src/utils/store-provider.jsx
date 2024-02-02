import React from 'react'

import store from './store.js';


export default ({reducers, children}) => {
  const {Provider} = store;
  const [state, dispatch] = React.useReducer(reducers, undefined, reducers);
  return <Provider value={{state, dispatch}}>{children}</Provider>;
};