import React from 'react';
import * as Icons from '@primer/octicons-react';
import {toast} from 'react-toastify';


export let error = (msg, opts = {}) => toast.error(msg, {icon: <Icons.AlertIcon />, theme: 'dark', ...opts});
export let info = (msg, opts = {}) => toast.info(msg, {icon: <Icons.SyncIcon />, theme: 'dark', ...opts});
export let success = (msg, opts = {}) => toast.success(msg, {icon: <Icons.CheckIcon />, theme: 'dark', ...opts});

export let dismiss = id => toast.dismiss(id);