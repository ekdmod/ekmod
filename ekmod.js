/* Image gallery items navigation */
(function (win) {
	
	var global = win;
	var doc = global.document;
	
	var dom = function(params,context) {
		return new GatMak(params,context);
	};
	
	var GatMak = function(params,context) {
		
	};
		
	window.onload = function(){
		
		var document = window.document;
		var previous = document.getElementById('image-left');
		var next = document.getElementById('image-right');
		var classElements = ['image-content', 'image-content-to-right', 'break-nav-right'];
		var classItems = findElementsByClassName(classElements);
		var context = classItems[0];
		var image_content_right = classItems[1];
		var break_list_right = classItems[2];
		
		if (context != null) {
			var glide = image_content_right.offsetWidth;
			var getLeft = 0;
			break_list_right.style.left = "0px";
			context.addEventListener('click',contextClick, false);
			var endItem = -image_content_right.scrollLeftMax;
			endItem > glide ? next.style.visibility = "invisible" : '';
		}
		
		// context != null ? context.addEventListener('click',contextClick, false) : '';
		
		function contextClick(event) {
			getLeft = parseInt(break_list_right.style.left.slice(0, -2));
			switch(event.target.id) {
				case 'image-left' :
					break_list_right.offsetLeft < 0 ? break_list_right.style.left = getLeft + glide + "px" : null; 
					break;
				case 'image-right-hide' :
					console.log(event.target.id);
					break;
				case 'image-right' :
					getLeft > endItem ? break_list_right.style.left = getLeft - glide + 'px' : null;
					break;
			}
		}		
		
		function getOverflowType(element) {
			return isOverflowed(element);
		}
	
		function isOverflowed(element) {
			return element.scrollHeight > element.clientHeight || element.scrollWidth > element.clientWidth;
		}
		
		function countSemiVisible(element) {
			var invisibleItems = [];
			for(var i=0; i<element.childElementCount; i++){
			  if (element.children[i].offsetTop + element.children[i].offsetHeight >
			      element.offsetTop + element.offsetHeight ||
			      element.children[i].offsetLeft + element.children[i].offsetWidth >
			      element.offsetLeft + element.offsetWidth ){
	
			        invisibleItems.push(element.children[i]);
			    }
	
			}
		}
		
		function findElementsByClassName(matchClass) {
		    var elements = document.getElementsByTagName('*'), i;
		    var classListItems = [];
		    for (i in elements) {
		    	for(cl in matchClass) {
		    		if((' ' + elements[i].className + ' ').indexOf(' ' + matchClass[cl] + ' ') > -1) {
			            classListItems.push(elements[i]);
			        }
		    	}
		    }
		    return classListItems;
		}
		
	}
})(window);