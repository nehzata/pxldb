import React from 'react';
import {PlanService} from 'pg-explain';

import Chart from './Chart';
import Graph from './Graph';
import * as C from './consts';
import * as utils from './utils';

const ps = new PlanService();
const tabs = [
  'graph',
  'time',
  'rows',
  'estimation',
  'cost',
  'raw',
];

const Plan = ({query, plan: planStr}) => {
  const [view, setView] = React.useState('graph');

  const [rootNode, plan] = React.useMemo(() => {
    try {
      const pj = ps.fromSource(planStr);
      const res = ps.createPlan("", pj, query);
      const planStats = {
        executionTime: Number(res.content[C.EXECUTION_TIME]) || Number(res.content[C.TOTAL_RUNTIME]) || NaN,
        planningTime: Number(res.content[C.PLANNING_TIME]) || NaN,
        maxRows: res.content.maxRows || NaN,
        maxCost: res.content.maxCost || NaN,
        maxDuration: res.content.maxDuration || NaN,
        maxBlocks: res.content.maxBlocks || {},
        maxIo: res.content.maxIo || NaN,
        triggers: res.content.Triggers || [],
        jitTime: (res.content.JIT && res.content.JIT.Timing && res.content.JIT.Timing.Total) || NaN,
      };
      planStats.triggerTime = planStats.triggers.reduce((acc, t) => (acc + t.Time), 0);
      return [
        pj.Plan,
        {
          ...res,
          planStats,
        },
      ];
    } catch (e) {
      return [null, null];
    }
  }, [query, planStr]);

  const content = React.useMemo(() => {
    if (view === 'raw') {
      return (
        <div className='text-xs'>
          <pre>
            {planStr}
          </pre>
        </div>
      );
    } else if (view === 'graph') {
      return (
        <div className='w-full h-[350px]'>
          <Graph rootNode={rootNode} plan={plan} />
        </div>
      );
    } else {
      return (
        <Chart plan={plan} metric={view} />
      );
    }
  }, [view, plan]);

  if (plan === null) return null;

  return (
    <>
      <div className='flex flex-row justify-start items-end mb-1'>
        <div className='flex-grow-0 flex-shrink-0 border-b border-gray-300' />
        {tabs.map((t, i) => (
          <button
            type='button'
            className={view === t
              ? 'px-2 py-1 border-b border-black font-semibold'
              : 'px-2 py-1 border-b text-gray-400 border-gray-300'
            }
            onClick={() => setView(t)}
            key={i}
          >{t}</button>
        ))}
        <div className='flex-grow border-b border-gray-300' />
      </div>
      <div className='flex flex-row justify-start items-center my-2 space-x-4'>
        <div className='text-sm text-gray-400'>
          Execution time: {plan.planStats.executionTime ? utils.duration(plan.planStats.executionTime) : 'N/A'}
        </div>
        <div className='text-sm text-gray-400'>
          Planning time: {plan.planStats.executionTime ? utils.duration(plan.planStats.planningTime) : 'N/A'}
        </div>
        <div className='text-sm text-gray-400'>
          Triggers: {plan.planStats.triggerTime ? utils.duration(plan.planStats.triggerTime) : 'N/A'}
        </div>
      </div>
      {content}
    </>
  );
};

export default Plan;