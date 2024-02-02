// Cynhyrchwyd y ffeil hon yn awtomatig. PEIDIWCH Â MODIWL
// This file is automatically generated. DO NOT EDIT
import {app} from '../models';

export function Cfg_DbsDelete(arg1:number):Promise<void>;

export function Cfg_DbsGet():Promise<Array<app.DB>>;

export function Cfg_DbsInsert(arg1:string,arg2:string):Promise<number>;

export function Cfg_DbsSetIsExpanded(arg1:number,arg2:boolean):Promise<void>;

export function Cfg_DbsUpdate(arg1:number,arg2:string,arg3:string):Promise<void>;

export function Cfg_GetSessionQueries(arg1:string):Promise<Array<app.SessionQuery>>;

export function Cfg_SessionQueriesDelete(arg1:number):Promise<void>;

export function Cfg_SessionQueriesInsert(arg1:string,arg2:string,arg3:string):Promise<number>;

export function Cfg_SessionQueriesUpdate(arg1:number,arg2:string,arg3:string):Promise<void>;

export function Cfg_SessionsDelete(arg1:string):Promise<void>;

export function Cfg_SessionsGet():Promise<Array<app.Session>>;

export function Cfg_SessionsInsert(arg1:number):Promise<string>;

export function Cfg_WinBoundsGet():Promise<app.WinBounds>;

export function Cfg_WinBoundsSet(arg1:app.WinBounds):Promise<void>;

export function Dbs_GetSchema(arg1:string,arg2:boolean):Promise<app.Schema>;

export function Dbs_Query(arg1:string,arg2:string):Promise<app.Result>;

export function Llm_Autocomplete(arg1:string,arg2:number,arg3:number,arg4:string):Promise<void>;

export function Llm_Init():Promise<void>;

export function Llm_Prime(arg1:number,arg2:string):Promise<void>;

export function Llm_SetSchema(arg1:number,arg2:string):Promise<void>;
