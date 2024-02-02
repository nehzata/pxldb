export default (state = {leftbar: true, rightbar: true}, action) => {
  switch (action.type) {
    case 'MISC-LEFTBAR-TOGGLE':
      return {...state, leftbar: !state.leftbar};
    case 'MISC-RIGHTBAR-TOGGLE':
      return {...state, rightbar: !state.rightbar};
    default:
      return state;
  }
};
