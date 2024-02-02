import React from 'react';

import {X} from '@pxl/atoms/Icons';
import Portal from '@pxl/atoms/Portal';


/*** https://tailwindui.com/components/application-ui/overlays/modals ***/
const Modal = ({open, children, onClose}) => {
  const onDocumentKey = React.useCallback(e => {
    if (e.key === 'Escape') {
      onClose && onClose();
    }
  }, [onClose]);
  React.useEffect(() => {
    document.addEventListener('keyup', onDocumentKey);
    return () => {
      document.removeEventListener('keyup', onDocumentKey);
    };
  }, [onDocumentKey]);
  if (open !== true) return null;
  return (
    <Portal>
      <div className='relative z-50'>
        <div
          className='fixed inset-0 bg-control dark:bg-control-dark bg-opacity-60 dark:bg-opacity-60 transition-opacity'
        />
        <div
          className='fixed z-50 inset-0 overflow-y-auto'
        >
          <div className='flex items-center justify-center min-h-full text-center'>
            <div
              className='relative bg-gray-100 dark:bg-canvas-dark border border-black border-opacity-20 dark:border-white dark:border-opacity-20 rounded-2xl shadow-2xl overflow-hidden w-full sm:max-w-md p-5'
            >
              {children}
            </div>
          </div>
        </div>
      </div>
    </Portal>
  );
};

//export let ModalBody = ({title, icon, children}) => (
//  <div className='bg-white px-4 pt-5 pb-4 sm:p-6 sm:pb-4'>
//    <div className='sm:flex sm:items-start'>
//      {icon && (
//        <div className='mx-auto flex-shrink-0 flex items-center justify-center h-12 w-12 rounded-full sm:mx-0 sm:h-10 sm:w-10'>
//          {icon}
//        </div>
//      )}
//      <div className={icon ? 'mt-3 text-center sm:mt-0 sm:ml-4 sm:text-left w-full' : 'mt-3 text-center sm:mt-0 sm:text-left w-full'}>
//        {title && <h3 className='text-lg leading-6 font-medium text-gray-900'>{title}</h3>}
//        <div className='mt-2'>
//          {children}
//        </div>
//      </div>
//    </div>
//  </div>
//);

//export let ModalFooter = ({children}) => (
//  <div className='bg-gray-50 px-4 py-3 sm:px-6 sm:flex sm:flex-row-reverse'>
//    {children}
//  </div>
//);

export let ModalBasic = ({open, onClose, children}) => (
  <Modal open={open} onClose={onClose}>
    <button
      className='float-left focus:outline-none p-3'
      onClick={onClose}
    >
      <X width={16} height={16} />
    </button>
    {children}
  </Modal>
);

export default Modal;
