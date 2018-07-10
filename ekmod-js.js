function urlparam( param )
{
	name = param.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
//	url = url.replace(/&amp;/, "&");
	var regexS = "[\\?&]"+name+"=([^&#]*)";  
	var regex = new RegExp( regexS );  
	var results = regex.exec( window.location.href ); 
	if( results == null )    return "";  
	else    return results[1];
}			

function JSNloa(XMLOBJid)	//http://dvcs.w3.org/hg/xhr/raw-file/tip/Overview.html
{
	ll = Ajax_kalba();
	ll.onreadystatechange=function()
	{
		if (ll.readyState==4 )
		{
			if (ll.status == 200 /*|| window.location.href.indexOf("http")==-1*/) {

				
				XMLOBJid['responder'] = ll.responseText;
				XMLOBJid['fn'](/*ll.responseText;ll.responseXML*/XMLOBJid);

			}
			else {XMLOBJid['fn'](ll.statusText)/*alert(ll.statusText);*/};
		}
	}
	ll.open( XMLOBJid['type'] , XMLOBJid['link'],false),
	
	ll.setRequestHeader('X-Requested-With', 'XMLHttpRequest'),
	/*ll.setRequestHeader( "Content-Type" , XMLOBJid['contentType'] ), ll.setRequestHeader( "If-Modified-Since" ,XMLOBJid['lastModified'] ), ll.setRequestHeader( "If-None-Match" , 1 ), */
	//var ct = xhr.getResponseHeader("content-type") || "";
	ll.setRequestHeader('link',XMLOBJid['l']),
	ll.send(XMLOBJid['data']);
	///*.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');*/  
}

function Ajax_kalba() {
	var xmlhttp;	
	
	if (!xmlhttp && typeof XMLHttpRequest != 'undefined') {
		try {
			xmlhttp = new XMLHttpRequest();
		} catch (e) {
			xmlhttp = false;
		}
	}

	return xmlhttp;

	/*	}
				}
	 */
}
			
function rawurlencode (str) {
	
	str = (str + '').toString(); 
	
	return encodeURIComponent(str).replace(/!/g, '%21').replace(/'/g, '%27').replace(/\(/g, '%28').
	replace(/\)/g, '%29').replace(/\*/g, '%2A');
}			