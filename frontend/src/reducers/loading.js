export default (state = {loading: true, error: null, progress: 0}, action) => {
  switch (action.type) {
    case 'LOADING-DONE':
      return {...state, loading: false};
    case 'LOADING-ERROR':
      return {...state, error: action.code};
    case 'LOADING-UPDATE':
      return {...state, progress: action.progress};
    default:
      return state;
  }
};
