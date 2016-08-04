(function(e,l,d){var g={callback:function(u){}};var j=/^(.+)\[(.+)\]$/,c=/^[0-9]+$/,a=/^\-?[0-9]+$/,t=/^\-?[0-9]*\.?[0-9]+$/,o=/[\u4e00-\u9fa5]/g,k=/^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,6}$/i,b=/^[a-z]+$/i,i=/^[a-z0-9]+$/i,m=/^[a-z0-9_-]+$/i,n=/^[0-9]+$/i,h=/^[1-9][0-9]*$/i,r=/^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[0-9]{1,2})$/i,f=/[^a-zA-Z0-9\/\+=]/i,q=/[!"#\$￥%&'\(\)\*\+,-\./0-9:;<=>\?@A-Z\[\\\]\^_`a-z{\|}~]{8,}/,cq=/[!"#\$￥%&'\(\)\*\+,-\./0-9:;<=>\?@A-Z\[\\\]\^_`a-z{\|}~]*/,s=/^\s*([01]?\d|2[0-3]):([0-5]\d)\s*$/i;var p=function(u,v){this.callback=v||g.callback;this.content=u;this.handlers={}};p.prototype.registerCallback=function(u,v){if(u&&typeof u==="string"&&v&&typeof v==="function"){this.handlers[u]=v}return this};p.prototype.validate_field=function(B,x,D){if(typeof x==="undefined"){return null}var C=x.split("|");if(x.indexOf("required")===-1&&(!B||B===""||B===d)){return null}var u="";var w="";var z=false;for(var y=0,v=C.length;y<v;y++){u=C[y];w="",z=false;if(parts=j.exec(u)){u=parts[1];w=parts[2]}if(typeof this._hooks[u]==="function"){if(!this._hooks[u].apply(this,[B,w])){z=true}}else{if(u.substring(0,9)==="callback_"){u=u.substring(9,u.length);if(typeof this.handlers[u]==="function"){var A=this.handlers[u].apply(this,[B]);if(A){z=true;return A}}}}if(false==z){continue}break}if(z){var A={};A.resId=D;A.errId="Menu."+u+"_err";A.arg1=w;A.arg2="";A.arg3="";A.arg4="";return A}return null};p.prototype._hooks={required:function(u){return(u!==null&&u!=="")},valid_email:function(u){return k.test(u)},valid_emails:function(w){var u=w.split(",");for(var v=0;v<u.length;v++){if(!k.test(u[v])){return false}}return true},min_length:function(v,u){if(!c.test(u)){return false}return(v.length>=parseInt(u,10))},max_length:function(v,u){if(!c.test(u)){return false}return(v.length<=parseInt(u,10))},exact_length:function(v,u){if(!c.test(u)){return false}return(v.length===parseInt(u,10))},greater_than:function(u,v){if(!t.test(u)){return false}return(parseFloat(u)>parseFloat(v))},less_than:function(u,v){if(!t.test(u)){return false}return(parseFloat(u)<parseFloat(v))},lenrange:function(w,y){var x=y.split("~");var v=parseFloat(x[0]);var u=parseFloat(x[1]);if(w.length>u){return false}else{if(w.length<v){return false}}return true},chinese:function(u,v){if(o.test(u)){return false}return true},range:function(x,z){if(!t.test(x)){return false}var y=z.split("~");var w=parseFloat(y[0]);var u=parseFloat(y[1]);var v=parseFloat(x);if(v>u){return false}else{if(v<w){return false}}return true},alpha:function(u){return(b.test(u))},alpha_numeric:function(u){return(i.test(u))},alpha_dash:function(u){return(m.test(u))},numeric:function(u){return(t.test(u))},integer:function(u){return(a.test(u))},decimal:function(u){return(t.test(u))},is_natural:function(u){return(n.test(u))},is_natural_no_zero:function(u){return(h.test(u))},valid_ip:function(u){return(r.test(u))},valid_base64:function(u){return(f.test(u))},valid_ssid:function(v){var w=/^[\x21-\x7e]*$/;var u=/.*[\x21-\x7e]$/;return(w.test(v)&&u.test(v))},time:function(u){return(s.test(u))},valid_pwd:function(u){return(q.test(u))},valid_pwd_custom_len:function(v,u){if(!c.test(u)){return false}if(v.length<parseInt(u,10)){return false}return(cq.test(v))},tr140username:function(u){var illegal="\\/:*?\"<>|.";for(var i=0;i<u.length;i++){if( (illegal.indexOf(u.charAt(i)) >=0 ) || (u.charCodeAt(i) <=32) ){return false}}return true;},integer_bzero:function(u){return c.test(u);}};e.FormValidator=p})(window,document);
