import config from '../config.js';

import cfg from './cfg';
import dbs from './dbs';
import llm from './llm';
import misc from './misc';
import sessions from './sessions';
import tabs from './tabs';


const combineReducers = (reducers, verbose = false) =>
  (state = {}, action = {}) => {
    const actions = action instanceof Array ? action : [action];
    verbose === true && console.group('action');
    verbose === true && console.log('action:', action);
    verbose === true && console.log('current state:', state);
    const res = actions.reduce((acc, actn) =>
      Object.entries(reducers).reduce(
        (acc, [k, v]) => ({
          ...acc,
          [k]: v(acc[k], actn),
        }),
        acc,
      ),
      state,
    );
    verbose === true && console.log('new state:', res);
    verbose === true && console.groupEnd('action');
    return res;
  };


export default combineReducers({
  cfg,
  dbs,
  llm,
  misc,
  sessions,
  tabs,
}, config.DEBUG_ENABLED);

