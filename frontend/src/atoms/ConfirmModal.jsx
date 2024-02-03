import React from 'react';
import * as Icons from 'untitledui-js-base';
import Modal from '@pxl/atoms/Modal';


export default ({btn, btnClassName = 'focus:outline-none', title, text, onConfirm}) => {
  const [open, setOpen] = React.useState(false);
  const _onConfirm = React.useCallback(e => {
    e.preventDefault();
    onConfirm();
    setOpen(false);
  }, [setOpen, onConfirm]);
  return (
    <>
      <button
        type='button'
        className={btnClassName}
        onClick={() => setOpen(true)}
      >{btn}</button>
      <Modal open={open} onClose={setOpen.bind(null, false)}>
        <div className='flex flex-col text-center space-y-4'>
          <div className='mx-auto text-red-500'>
            <Icons.AlertOctagon size={32} stroke='currentColor' />
          </div>
          <h3 className='font-semibold text-gray-700 dark:text-gray-200 text-sm'>{title}</h3>
          <div className='mt-2 dark:text-gray-300 text-sm'>{text}</div>
          <div className='flex flex-row justify-between items-center space-x-2'>
            <button
              className='flex-grow rounded-lg border shadow-sm px-4 py-1 text-black bg-white hover:bg-gray-100 text-sm'
              onClick={setOpen.bind(null, false)}
            >Cancel</button>
            <button
              className='flex-grow rounded-lg border dark:border-blue-500 shadow-sm px-4 py-1 text-white bg-blue-500 hover:bg-blue-400 text-sm'
              onClick={_onConfirm}
            >Yes</button>
          </div>
        </div>
      </Modal>
    </>
  );
};