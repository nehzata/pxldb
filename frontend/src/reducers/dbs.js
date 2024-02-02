const index = list => {
  list.sort((a, b) => {
    if (a.name < b.name) return -1;
    else if (a.name > b.name) return 1;
    return 0;
  });
  return {
    index: list.reduce((acc, s, i) => ({...acc, [s.id]: i}), {}),
    list,
  };
};

export default (state = {index: {}, list: []}, action) => {
  switch (action.type) {
    case 'DBS-SET':
      {
        const {dbs} = action;
        return index(dbs.map(db => ({...db, schema: null})));
      }
    case 'DBS-ADD':
      {
        const {id, dsn, name} = action;
        const newDbs = [
          ...state.list.filter(db => db.id !== id),
          {id, dsn, name, schema: null},
        ];
        return index(newDbs);
      }
    case 'DBS-UPDATE':
      {
        const {id, dsn, name} = action;
        return {
          ...state,
          list: state.list.map(db => db.id !== id
            ? db
            : {...db, dsn, name}
          ),
        };
      }
    case 'DBS-DELETE':
      {
        const {id} = action;
        const newDbs = state.list.filter(db => db.id !== id);
        return index(newDbs);
      }
    case 'DBS-SET-IS-EXPANDED':
      {
        const {id, isExpanded} = action;
        return {
          ...state,
          list: state.list.map(db => db.id !== id
            ? db
            : {...db, isExpanded}
          ),
        };
      }
    case 'DBS-SET-SCHEMA':
      {
        const {id, schema} = action;
        return {
          ...state,
          list: state.list.map(db => db.id !== id
            ? db
            : {...db, schema}
          ),
        };
      }
    default:
      return state;
  }
};
