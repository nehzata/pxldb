const APP_ENV = import.meta.env.VITE_APP_ENV || 'local';
const DEBUG_ENABLED = ((import.meta.env.VITE_DEBUG_ENABLED || 'true') === 'true');
const VERSION = import.meta.env.VITE_VERSION || 'local';

export default {
  APP_ENV,
  VERSION,

  DEBUG_ENABLED,
};
