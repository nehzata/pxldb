import React from 'react';
import {useLocation} from 'react-router-dom';

const ctx = React.createContext();

const _track = async (TRACKING_URL, ID, type, params) => {
  const {
    screen: {width, height},
    location: {hostname},
    navigator: {language},
  } = window;
  try {
    await fetch(TRACKING_URL, {
      method: 'PUT',
      headers: {
        'content-type': 'application/json',
      },
      body: JSON.stringify({
        type,
        payload: {
          website: ID,
          hostname,
          screen: [width, height].join('x'),
          language,
          ...params,
        },
      }),
    });
  } catch (e) {
    console.error(e);
  }
};

export let tracking = ctx;

export let TrackingProvider = ({store, children}) => {
  const {Provider} = ctx;
  const [referrer, setReferrer] = React.useState(window.document.referrer);
  const {
    state: {
      cfg: {TRACKING_URL, TRACKING_ID},
    },
  } = React.useContext(store);
  const location = useLocation();

  const trackEvent = React.useMemo(() => {
    return (eventType, eventValue) =>
      _track(TRACKING_URL, TRACKING_ID, 'event', {
        event_type: eventType,
        event_value: eventValue,
      });
  }, [TRACKING_URL, TRACKING_ID]);

  React.useEffect(() => {
    setReferrer((r) => {
      const url = [location.pathname, location.search, location.hash].join('');
      _track(TRACKING_URL, TRACKING_ID, 'pageview', {url, referrer: r});
      return url;
    });
  }, [TRACKING_URL, TRACKING_ID, location]);

  return <Provider value={trackEvent}>{children}</Provider>;
};
