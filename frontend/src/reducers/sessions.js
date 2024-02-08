const index = list => ({
  index: list.reduce((acc, s, i) => ({...acc, [s.id]: i}), {}),
  list,
});

export default (state = {index: {}, list: []}, action) => {
  switch (action.type) {
    case 'SESSIONS-SET':
      {
        const {sessions} = action;
        return index(sessions.map(s => ({...s, queries: null})));
      }
    case 'SESSIONS-ADD':
      {
        const {id, dbId, title} = action;
        const newSessions = [
          ...state.list.filter(s => s.id !== id),
          {id, dbId, title, queries: null},
        ];
        return index(newSessions);
      }
    case 'SESSIONS-UPDATE':
      {
        const {id, title} = action;
        return {
          ...state,
          list: state.list.map(s => s.id !== id
            ? s
            : {...s, title},
          ),
        };
      }
    case 'SESSIONS-DELETE':
      {
        const {id} = action;
        const newSessions = state.list.filter(s => s.id !== id);
        return index(newSessions);
      }
    case 'SESSION-QUERIES-SET':
      {
        const {sId, queries} = action;
        const session = state.list[state.index[sId]];
        const newSession = {
          ...session,
          queries: queries.map(q => ({
            ...q,
            isResStale: true,
            isExplainStale: true,
            isAnalyzeStale: true,
            isLoading: false,
          })),
        };
        return {
          ...state,
          list: state.list.map(s => s.id === sId ? newSession : s),
        };
      }
    case 'SESSION-QUERIES-UPDATE':
      {
        const {sId, id, qry, res, explain, analyze} = action;
        const session = state.list[state.index[sId]];
        const {found, list: newQueries} = session.queries.reduce((acc, q) => {
          if (q.id === id) {
            acc.found = true;
            acc.list.push({
              ...q,
              id,
              qry,
              res: res !== undefined ? res : q.res,
              explain: explain !== undefined ? explain : q.explain,
              analyze: analyze !== undefined ? analyze : q.analyze,
              isResStale: res !== undefined ? false : q.isResStale,
              isExplainStale: explain !== undefined ? false : q.isExplainStale,
              isAnalyzeStale: analyze !== undefined ? false : q.isAnalyzeStale,
            });
          } else {
            acc.list.push(q);
          }
          return acc;
        }, {found: false, list: []});
        if (found !== true) {
          newQueries.push({
            id,
            qry,
            res,
            explain,
            analyze,
            isResStale: false,
            isExplainStale: false,
            isAnalyzeStale: false,
          });
        }
        return {
          ...state,
          list: state.list.map(s => s.id === sId
            ? {...s, queries: newQueries}
            : s
          ),
        };
      }
    case 'SESSION-QUERIES-SET-LOADING':
      {
        const {sId, id, isLoading} = action;
        const session = state.list[state.index[sId]];
        const newQueries = session.queries.reduce((acc, q) => {
          if (q.id === id) {
            acc.push({
              ...q,
              isLoading,
            });
          } else {
            acc.push(q);
          }
          return acc;
        }, []);
        return {
          ...state,
          list: state.list.map(s => s.id === sId
            ? {...s, queries: newQueries}
            : s
          ),
        };
      }
    case 'SESSION-QUERIES-DELETE':
      {
        const {id, sId} = action;
        const session = state.list[state.index[sId]];
        const newSession = {
          ...session,
          queries: session.queries.filter(q => q.id != id),
        };
        return {
          ...state,
          list: state.list.map(s => s.id === sId ? newSession : s),
        };
      }
    default:
      return state;
  }
};
