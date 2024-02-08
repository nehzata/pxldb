import React from 'react';
import * as Icons from 'untitledui-js-base';

import Plan from '@pxl/components/Plan';
import Tabs from './Tabs';


const samplePlan = `Gather  (cost=1000.56..24192.07 rows=29 width=201) (actual time=47.786..50.222 rows=25 loops=1)
  Workers Planned: 2
  Workers Launched: 2
  ->  Nested Loop  (cost=0.56..23189.17 rows=12 width=201) (actual time=39.557..42.364 rows=8 loops=3)
      ->  Parallel Seq Scan on repos r  (cost=0.00..23156.33 rows=1 width=121) (actual time=39.537..42.336 rows=0 loops=3)
            Filter: (name = 'thanksd'::text)
            Rows Removed by Filter: 324723
      ->  Index Scan using repo_dependencies_depender_type_depender_gid_dependee_type_key1 on repo_dependencies rd  (cost=0.56..32.70 rows=14 width=80) (actual time=0.053..0.070 rows=25 loops=1)
            Index Cond: ((depender_type = r.repo_type) AND (depender_gid = r.repo_gid))
Planning Time: 0.574 ms
Execution Time: 50.330 ms
`;

const ResultPlan = ({mode, onChangeView, onLoad, onExport, query, error = null, rows = null, isStale, isLoading, hasError}) => {
  const plan = React.useMemo(() => {
    if (rows === null || !rows) {
      return null;
    }
    return rows.join('\n');
  }, [rows]);

  React.useEffect(() => {
    if (rows === null) {
      onLoad();
    }
  }, []);

  return (
    <div>
      <div className={isStale
        ? 'relative border rounded-l-md rounded-tr-md shadow-sm overflow-x-scroll p-1 text-sm text-gray-900 max-w-full bg-gray-100'
        : 'relative border rounded-l-md rounded-tr-md shadow-sm overflow-x-scroll p-1 text-sm text-gray-900 max-w-full'
      }>
        {error !== null && (
          <span>{error}</span>
        )}
        {error === null && (
          <div className={isLoading ? 'blur-[2px]' : ''}>
            <button
              className='absolute top-0 right-0 text-gray-400 p-1'
              onClick={onLoad}
              disabled={isLoading}
              title='Refresh plan'
            >
              {!isLoading && (
                <Icons.RefreshCCW01 size={20} stroke='currentColor' />
              )}
              {isLoading && (
                <Icons.RefreshCCW03 size={20} stroke='currentColor' />
              )}
            </button>
            <Plan query={query} plan={plan} />
            {/* <Plan query={query} plan={samplePlan} /> */}
          </div>
        )}
      </div>
      <div className='flex flex-row justify-end items-center leading-none text-gray-500 text-sm ml-3'>
        <Tabs onSelect={onChangeView} onExport={onExport} active={mode} hasError={hasError} />
      </div>
    </div>
  );
};

export default ResultPlan;

