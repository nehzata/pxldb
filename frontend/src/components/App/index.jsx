import React from 'react';

import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';

import Loading from '@pxl/components/Loading';
import Leftbar from '@pxl/components/Leftbar';
import Rightbar from '@pxl/components/Rightbar';
import Body from '@pxl/components/Body';
import './index.css';

export default () => {
  const {state: {loading: {loading}}, dispatch} = React.useContext(store);
  const [state, setState] = React.useState(null);

  React.useEffect(() => {
    (async () => {
      try {
        const [dbs, sessions] = await Promise.all([
          Wails.Cfg_DbsGet(),
          Wails.Cfg_SessionsGet(),
        ]);
        dispatch([
          {
            type: 'SESSIONS-SET',
            sessions,
          },
          {
            type: 'DBS-SET',
            dbs,
          },
        ]);
        setState(true);
      } catch (err) {
        console.log(err);
        setState(false);
      }
    })();
  }, []);

  React.useEffect(() => {
    // Wails.Llm_Init();
    //   const {pxl} = window;
    //   pxl.onDispatch(dispatch);
    //   pxl.onOpen(id => {
    //     dispatch({type: 'TABS-OPEN', id});
    //   });
    //   // dispatch({type: 'TABS-OPEN', id: '2d944971-81f5-4213-aa06-4a5e7ba64651'});
    // dispatch({type: 'TABS-OPEN', id: 'a1cb3dd9-87e9-47f2-93c8-94468129648d'});
  }, []);


  if (state === null) {
    return <div className='relative w-full h-full bg-black' style={{'--wails-draggable': 'drag'}} />;
  } else if (loading === true) {
    return <Loading />;
  }

  return (
    <div className='relative w-full h-full z-10'>
      <Leftbar />
      <Rightbar />
      <Body />
    </div>
  );
};