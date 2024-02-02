let cb = null;

export const publish = v => {
  if (cb !== null) {
    cb(v);
  }
};

export const subscribe = _cb => {
  cb = _cb;
};

export const unsubscribe = _cb => {
  if (cb === _cb) {
    cb = null;
  }
};