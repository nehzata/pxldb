import React from 'react';
import {createPortal} from 'react-dom';


const Portal = ({children}) => {
  const el = React.useMemo(() => document.createElement('div'), []);
  React.useEffect(() => {
    document.body.appendChild(el);
    return () => document.body.removeChild(el);
  }, []);
  return createPortal(children, el);
}

export default Portal;