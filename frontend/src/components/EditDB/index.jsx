import React from 'react';
import {useForm} from 'react-hook-form';
import * as Icons from 'untitledui-js-base';

import store from '@pxl/utils/store.js';
import Modal from '@pxl/atoms/Modal';
import ConfirmModal from '@pxl/atoms/ConfirmModal';
import * as Wails from '@wails/go/app/App';


const DNS_REGEX = /^postgres:\/\/(((?<username>.*)(:(?<passwd>.*))?)@)?(?<host>.+)(:(?<port>\d*))?\/(?<dbname>[^\?]+)(\?(?<qs>.*))?/;

const EditDB = ({id, dsn, name, open, onClose}) => {
  const {
    state: {
      sessions: {list: sessions},
    },
    dispatch,
  } = React.useContext(store);
  const {
    register,
    trigger,
    handleSubmit,
    formState: {isValid},
    getValues,
    setValue,
  } = useForm({mode: 'onChange', defaultValues: {dsn, name}});

  const onBlur = React.useCallback(e => {
    const dsn = getValues('dsn');
    const cn = getValues('name');
    if (cn === null || cn.trim() === '') {
      const r = DNS_REGEX.exec(dsn);
      if (r !== null) {
        const {dbname} = r.groups;
        setValue('name', dbname);
        trigger();
      }
    }
  }, [getValues, setValue]);

  const onSubmit = React.useCallback(({dsn, name}) => {
    if (id === null) {
      (async () => {
        const newDbId = await Wails.Cfg_DbsInsert(dsn, name);
        const newSessionId = await Wails.Cfg_SessionsInsert(newDbId);
        await Wails.Cfg_DbsSetIsExpanded(newDbId, true);
        dispatch([
          {type: 'DBS-ADD', id: newDbId, dsn, name},
          {type: 'DBS-SET-IS-EXPANDED', id: newDbId, isExpanded: true},
          {type: 'SESSIONS-ADD', id: newSessionId, dbId: newDbId, title: null},
          {type: 'TABS-OPEN', id: newSessionId},
        ]);
      })();
    } else {
      Wails.Cfg_DbsUpdate(id, dsn, name);
      dispatch({type: 'DBS-UPDATE', id, dsn, name});
    }
    onClose();
  }, []);

  const onDelete = React.useCallback(async () => {
    Wails.Cfg_DbsDelete(id);
    dispatch([
      ...sessions.reduce((acc, s) => {
        if (s.dbId === id) {
          acc.push({type: 'TABS-CLOSE', id: s.id});
        }
        return acc;
      }, []),
      {type: 'DBS-DELETE', id}
    ]);
    onClose();
  }, [id, onClose, sessions]);

  React.useEffect(() => {
    if (open === true) {
      setValue('dsn', dsn);
      setValue('name', name);
    }
  }, [open]);

  return (
    <Modal open={open} onClose={onClose}>
      <form onSubmit={handleSubmit(onSubmit)} className='flex flex-col text-center'>
        <div className='mx-auto text-gray-400'>
          <Icons.Database01 size={36} stroke='currentColor' />
        </div>
        <h3 className='font-semibold text-gray-700 dark:text-gray-300 text-sm mt-4'>{id === null ? 'Add database' : 'Edit database'}</h3>
        <div className='mt-4 flex flex-col space-y-4'>
          <div className='text-left'>
            <label className='px-2 text-gray-500 dark:text-gray-400 text-sm' htmlFor='dsn'>DSN</label>
            <input
              className='px-2 py-1 border shadow-sm rounded-lg w-full focus:outline-blue-200'
              placeholder='postgres://...'
              name='dsn'
              {...register(
                'dsn',
                {
                  required: {value: true, message: 'A DSN is required'},
                  pattern: {
                    value: DNS_REGEX,
                    message: 'A valid DSN is required',
                  },
                }
              )}
              onBlur={onBlur}
            />
          </div>
          <div className='text-left'>
            <label className='px-2 text-gray-500 dark:text-gray-400 text-sm' htmlFor='name'>Name</label>
            <input
              className='px-2 py-1 border shadow-sm rounded-lg w-full focus:outline-blue-200'
              name='name'
              {...register('name', {required: true})}
            />
          </div>
        </div>
        <div className='flex flex-row justify-between items-center space-x-2 mt-6 text-sm'>
          {id !== null && (
            <ConfirmModal
              btn='Delete'
              btnClassName='flex-grow rounded-lg border dark:border-control-dark shadow-sm px-4 py-1 text-white bg-red-500 hover:bg-red-400 focus:outline-none'
              title='Delete database?'
              text='Are you sure you wish to delete this connection?'
              onConfirm={onDelete}
            />
          )}
          <button
            type='button'
            className='flex-grow rounded-lg border dark:border-control-dark shadow-sm px-4 py-1 text-black dark:text-white bg-white dark:bg-gray-500 hover:bg-gray-100 dark:hover:bg-gray-700 focus:outline-none'
            onClick={onClose}
          >Cancel</button>
          <button
            type='submit'
            disabled={isValid !== true}
            className='flex-grow rounded-lg border dark:border-control-dark shadow-sm px-4 py-1 text-white bg-blue-500 hover:bg-blue-400 dark:hover:bg-blue-700 disabled:bg-blue-300 dark:disabled:bg-blue-400 focus:outline-none'
          >{id === null ? 'Add' : 'Edit'}</button>
        </div>
      </form>
    </Modal>
  );
}

export default EditDB;