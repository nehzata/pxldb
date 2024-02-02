import React from 'react';

import Input from './Input';
import ResultRows from './ResultRows';
import ResultPlan from './ResultPlan';

const Query = ({dbId, dsn, schema, sId, id, qry = '', res = null, isStale, isLoading}) => {
  return (
    <div className='flex flex-col space-y-2'>
      <Input dbId={dbId} dsn={dsn} schema={schema} sId={sId} id={id} qry={qry} />
      {res !== null && res.isPlan !== true && (
        <ResultRows id={id} isStale={isStale} isLoading={isLoading} {...res} />
      )}
      {res !== null && res.isPlan === true && (
        <ResultPlan query={qry} isStale={isStale} isLoading={isLoading} {...res} />
      )}
    </div>
  );
};

export default Query;