export default (state = {activeId: null, tabs: []}, action) => {
  switch (action.type) {
    case 'TABS-OPEN':
      {
        const {id, isPreview = true} = action;
        let {found, tabs: newTabs} = state.tabs.reduce((acc, t) => {
          if (t.id === id) {
            acc.found = true;
            acc.tabs.push({
              ...t,
              isPreview: false,
            });
          } else if (!t.isPreview) {
            acc.tabs.push(t);
          }
          return acc;
        }, {found: false, tabs: []});
        if (!found) {
          newTabs.push({
            id,
            isPreview,
          });
        }
        return {activeId: id, tabs: newTabs};
      }
    case 'TABS-CLOSE':
      {
        const {id} = action;
        const isActiveRemoved = (id === state.activeId);
        const {idBeforeActive, tabs} = state.tabs.reduce((acc, t) => {
          acc.isPostActive ||= (t.id === state.activeId);
          if (!acc.isPostActive) {
            acc.idBeforeActive = t.id;
          }
          if (t.id !== id) {
            acc.tabs.push(t);
          }
          return acc;
        }, {idBeforeActive: null, isPostActive: false, tabs: []});
        return {
          ...state,
          activeId: isActiveRemoved ? idBeforeActive : state.activeId,
          tabs,
        }
      }
    default:
      return state;
  }
};
