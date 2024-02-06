export default (state = {enabled: false, loaded: null, error: null, progress: 0, received: null, total: null}, action) => {
  switch (action.type) {
    case 'LLM-ENABLED':
      const {enabled} = action;
      return {...state, enabled};
    case 'LLM-LOADED':
      const {loaded} = action;
      return {...state, loaded};
    case 'LLM-ERROR':
      return {...state, error: action.code};
    case 'LLM-UPDATE':
      const {progress, received, total} = action;
      return {...state, progress, received, total};
    default:
      return state;
  }
};
