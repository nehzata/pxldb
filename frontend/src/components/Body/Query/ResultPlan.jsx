import React from 'react';
import Plan from '@pxl/components/Plan';


const plan = `Gather  (cost=1000.56..24192.07 rows=29 width=201) (actual time=47.786..50.222 rows=25 loops=1)
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

const ResultPlan = ({query, err = null, rows = [], isStale, isLoading}) => {
  if (err != null) {
    return (
      <div>
        <span className={isStale
          ? 'border rounded-md shadow-sm p-1 text-sm text-gray-900 bg-gray-100'
          : 'border rounded-md shadow-sm p-1 text-sm text-gray-900'
        }>{err}</span>
      </div>
    );
  }

  return (
    <div className={isStale
      ? 'inline-block border rounded-md shadow-sm overflow-x-scroll p-1 text-sm text-gray-900 max-w-full bg-gray-100'
      : 'inline-block border rounded-md shadow-sm overflow-x-scroll p-1 text-sm text-gray-900 max-w-full'
    }>
      <Plan query={query} plan={rows.join('\n')} />
      {/* <Plan query={query} plan={plan} /> */}
    </div >
  );
};

export default ResultPlan;

