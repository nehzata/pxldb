import React from 'react';

import store from '@pxl/utils/store.js';
import * as Wails from '@wails/go/app/App';
import Query from './Query';


const Session = () => {
  const {
    state: {
      dbs: {index: dbIndex, list: dbs},
      sessions: {index: sessionIndex, list: sessions},
      tabs: {activeId},
    },
    dispatch
  } = React.useContext(store);

  const [db, session, queries] = React.useMemo(() => {
    const session = sessions[sessionIndex[activeId]]
    const db = dbs[dbIndex[session.dbId]];
    const queries = session.queries !== null
      ? [...session.queries, {id: null, qry: '', res: null, isStale: true}]
      : null;
    return [db, session, queries];
  }, [activeId, dbs, sessions]);

  React.useEffect(() => {
    if (queries === null) {
      (async () => {
        const queries = await Wails.Cfg_GetSessionQueries(session.id);
        dispatch({
          type: 'SESSION-QUERIES-SET',
          sId: session.id,
          queries,
        });
      })();
    }
  }, [activeId]);

  React.useEffect(() => {
    if (db.schema !== null) {
      (async () => {
        const {tables} = db.schema;
        const sql = tables.map(({name, cols}) => ([
          `CREATE TABLE ${name} (`,
          cols.map(({desc}) => `\t${desc}`).join(',\n'),
          ');'
        ].join('\n'))).join('\n\n');
        await Wails.Llm_SetSchema(
          db.id,
          `<s>[INST] You are required to respond back with PostgreSQL queries based on the following schema:\n ${sql}\n\n[/INST]PostgreSQL query:\n`
        );
      })();
    }
  }, [db.id, db.schema]);

  if (queries === null) return null;

  return (
    <div className='overflow-scroll flex flex-col p-4 space-y-8'>
      {queries.map((q, i) => <Query dbId={db.id} dsn={db.dsn} schema={db.schema} sId={activeId} {...q} key={i} />)}
    </div>
  );
}

export default Session;