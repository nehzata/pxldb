export default (state = {loading: true, error: null, progress: 0, received: null, total: null}, action) => {
  switch (action.type) {
    case 'LOADING-DONE':
      return {...state, loading: false};
    case 'LOADING-ERROR':
      return {...state, error: action.code};
    case 'LOADING-UPDATE':
      const {progress, received, total} = action;
      return {...state, progress, received, total};
    default:
      return state;
  }
};
