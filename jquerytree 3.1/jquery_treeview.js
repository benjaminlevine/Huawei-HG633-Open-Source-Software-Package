/*
 * Treeview 1.4 - jQuery plugin to hide and show branches of a tree
 * 
 * http://bassistance.de/jquery-plugins/jquery-plugin-treeview/
 * http://docs.jquery.com/Plugins/Treeview
 *
 * Copyright (c) 2007 Jörn Zaefferer
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 *
 * Revision: $Id: jquery.treeview.js 4684 2008-02-07 19:08:06Z joern.zaefferer $
 *
 */
var maskStatus = 0;
var PageInfo={};

;(function($) {

	$.extend($.fn, {
		swapClass: function(c1, c2) {
			var c1Elements = this.filter('.' + c1);
			this.filter('.' + c2).removeClass(c2).addClass(c1);
			c1Elements.removeClass(c1).addClass(c2);
			return this;
		},
		loadMaskDiv: function(){
			if(maskStatus==0)
			{
				$("#maskContent").css({
					"opacity": "0.6"
				});
				$("#maskContent").fadeIn("normal");
				$("#maskWaiteImg").fadeIn("normal");
				maskStatus = 1;
			}
		},
		unloadmaskDiv: function(){
		    if (1 == maskStatus)
		    {
		    	$("#maskContent").fadeOut("normal");
				$("#maskWaiteImg").fadeOut("normal");
				maskStatus  = 0;
		    }
		},
		setMaskAndContent: function(){
			var ImgHeight = document.getElementById('maskWaiteImg').offsetHeight;
			var ImgWidth = document.getElementById('maskWaiteImg').offsetWidth;
			var t=(PageInfo.height()/2)-(ImgHeight/2); t=t<10?10:t;
			var topVal =(t+PageInfo.top()) + 'px';
			var leftVal =((PageInfo.width()/2)-(ImgWidth/2)) + 'px';
			
			/*var temp=(getElement('showContent').offsetHeight/2)-(getElement('browser').offsetHeight/2); temp=temp<10?10:temp;
			var topValTable =(temp+PageInfo.top()) + 'px';
			var leftValTable =(PageInfo.width()/2)-((getElement('browser').offsetWidth)/2) + 'px';*/

			$("#maskWaiteImg").css({
				"top": topVal,
				"left": leftVal
			});		

			/*$("#browser").css({
				"top": topValTable,
				"left": leftValTable
			});	*/

			$("#maskContent").css({
				"height": PageInfo.theight(),
				"width": PageInfo.twidth()
			});
		},
		resize:function(){
		    $(this).setMaskAndContent();
		},
		findTreePath: function(){
			var strPath = '';
            var AppendFlag = 0;
			//check class of the first child
			var ulLength = $(this).children().length;
			for (var z = 0; z < ulLength; z++)
			{
				if ($(this).children().eq(z).is('ul'))
				{
					break;
				}
			}
			//eq(0) is div
            if ($(this).children().eq(0).hasClass('hitarea'))
			{
		        if (false == $(this).children().eq(0).hasClass('collapsable-hitarea'))
				{
					if ($(this).children().eq(0).hasClass('expandable-hitarea'))
					{
						//remove child not remove the first child, the child of ul Item 
						$(this).children().eq(z).children().not(':first').remove();
					}
					AppendFlag = 1;
					//set radio checked
					$(this).setCurrentRadio();
					//set button enable
					$(this).setButtonEnable();
					return $(this).findCurrentPath(AppendFlag);
				}
		    }			
			if (false == ($(this).children().eq(0).hasClass('hitarea')))
			{
				//for one file folder
				AppendFlag = 1;
			}
			//set radio checked
			if ($(this).find('input').length > 0)
			{
				$(this).setCurrentRadio();
			}
			//set button enable
			$(this).setButtonEnable();
			return $(this).findCurrentPath(AppendFlag);
		},
		findCurrentPath: function(IsAppend)
		{
			var CurrentPath = '';
			var CurrentOldPath = '';
			var id          = '';
			var parentLength = 0;
			
			if (IsAppend)
			{
			    //only modify the path
				CurrentPath = 'notAppend#';
			}
			id = $(this).find('ul').attr('id');
			parentLength = $(this).parents().find(">span").length;
			if (parentLength == 0)
			{
				//for folder
				CurrentPath += '/' + $(this).find(">span").text();
				CurrentOldPath += '-' + $(this).find(">span").attr("id");
				CurrentPath = CurrentPath + "/*" + CurrentOldPath;
				return CurrentPath;
			}
			for (var j = (parentLength - 1); j >= 0; j--)
			{
				CurrentPath += '/' + $(this).parents().find(">span").eq(j).text();
				CurrentOldPath += '-' + $(this).parents().find(">span").eq(j).attr("id");
			}
			if ('' != $(this).find(">span").text())
			{
				CurrentPath += '/' + $(this).find(">span").text();
				CurrentOldPath += '-' + $(this).find(">span").attr("id");
			}
			CurrentPath = CurrentPath + "/*" + CurrentOldPath;
			return CurrentPath;

		},
		replaceClass: function(c1, c2) {
			return this.filter('.' + c1).removeClass(c1).addClass(c2).end();
		},
		hoverClass: function(className) {
			className = className || "hover";
			return this.hover(function() {
				$(this).addClass(className);
			}, function() {
				$(this).removeClass(className);
			});
		},
		heightToggle: function(animated, callback) {
			//this is ul item
			animated ?
				this.animate({ height: "toggle" }, animated, callback) :
				this.each(function(){
				    // this ul containt li and the li containt span, else the ul is virtual
					if ($(this).find('li').length > 0)
					{
						//for folder has file
						jQuery(this)[ jQuery(this).is(":hidden") ? "show" : "hide" ]();
					}
					if(callback)
						callback.apply(this, arguments);
				});
		},
		heightHide: function(animated, callback) {
			if (animated) {
				this.animate({ height: "hide" }, animated, callback);
			} else {
				this.hide();
				if (callback)
					this.each(callback);				
			}
		},
		prepareBranches: function(settings, toggler) {
			//this is li item
			if (!settings.prerendered) {
				// mark last tree items
				this.filter(":last-child:not(ul)").addClass(CLASSES.last);
				// collapse whole tree, or only those marked as closed, anyway except those marked as open
				this.filter((settings.collapsed ? "" : "." + CLASSES.closed) + ":not(." + CLASSES.open + ")").find(">ul").hide();
				this.filter(":not(:has(>a))").find(">span").click(toggler).add( $("a", this) ).hoverClass();
				//hide the first li for folder has children
				if ($(this).find('li').length > 0)
				{
					//if this content li and li not containt span, this node is virtual li node.
					if (false == ($(this).find('li').children().is('span')))
					{
						$(this).find('li').hide();
					}
				}
			}
			// return all items with sublists
			return this.filter(":has(>ul)");
		},
		applyClasses: function(settings, toggler) {	
		    //this is ul item
			if (!settings.prerendered) {
				// handle closed ones first
				this.filter(":has(>ul:hidden)")
						.addClass(CLASSES.expandable)
						.replaceClass(CLASSES.last, CLASSES.lastExpandable);
						
				// handle open ones
				this.not(":has(>ul:hidden)")
						.addClass(CLASSES.collapsable)
						.replaceClass(CLASSES.last, CLASSES.lastCollapsable);
				if ($(this).find('li').length != 0)	
				{
					// create hitarea
					this.prepend("<div class=\"" + CLASSES.hitarea + "\"/>").find("div." + CLASSES.hitarea).each(function()                {
						var classes = "";
						$.each($(this).parent().attr("class").split(" "), function() {
							classes += this + "-hitarea ";
						});
						$(this).addClass( classes );
					});
					// apply event to hitarea
					this.find("div." + CLASSES.hitarea).click( toggler );
				}
				else
				{
					//for virtual ul
					this.prepend("<div class=\"" + CLASSES.hitareaEx + "\"/>");
					this.find("div." + CLASSES.hitareaEx).click( toggler );
				}
			}
		},
		BuildcommonAppendStAndAppendOne: function(OldName, name, type, fileflag, firstflag, id, ulIdx)
		{
			var $common = '<span style="white-space:pre" id="' + OldName + '">' + name + '</span>';
			//set id is name
			var newid = 'usbId';
			var $tempFileImage = $fileimage;
			var $tempFolderImage = $folderimage;
			var $AppendSt = '';
			var $InputRadio = '<INPUT type="radio" name="treeRadio" id="treeRadio" value="' + g_radioValue + '">';
			
			if (!firstflag)
			{
				$tempFileImage = $diskimage;
				$tempFolderImage = $tempFileImage;
				if (ulIdx == 0)
				{
					$InputRadio = '<INPUT checked type="radio" name="treeRadio" id="treeRadio" value="' + g_radioValue + '">';
				}
			}
			if (firstflag && ulIdx == 0)
			{
				//set all radio is no checked
				$(':radio').removeAttr('checked');
				//set the first child is checked
				$InputRadio = '<INPUT checked type="radio" name="treeRadio" id="treeRadio" value="' + g_radioValue + '">';
			}
			g_radioValue++;
			switch(type)
			{
				case 0:
					 //file
					 if (fileflag)
					 {
						//if support file
						$AppendSt = $AppendSt + '<li>' + $tempFileImage + $InputRadio + $common + '<ul></ul></li>';
					 }
				break;
				case 1:
					//folder has no file
					$AppendSt = $AppendSt + '<li>' + $tempFolderImage + $InputRadio + $common + '<ul></ul></li>';
				break;
				case 2:
					 //folder has file only
					 if (fileflag)
					 {
						//if support file
						g_idNum++;
						$AppendSt = $AppendSt + '<li>' + $tempFolderImage + $InputRadio + $common + '<ul id=' + newid + '_' + g_idNum +'><li></li></ul></li>';
					 }
					 else
					 {
						$AppendSt = $AppendSt + '<li>' +  $tempFolderImage + $InputRadio + $common + '<ul></ul></li>';
					 }
				break;
				case 3:
					  g_idNum++;
					  $AppendSt = $AppendSt + '<li>' +  $tempFolderImage + $InputRadio + $common + '<ul id=' + newid + '_' + g_idNum +'><li></li></ul></li>';
				break;
				default:
				break;
			}
			if ('' != $AppendSt)
			{
				var branches = $($AppendSt).appendTo(id);
				$(id).treeview({
					add: branches
				});
			}
		},
		TransferBlanckToHtml: function(SrcStr)
		{
		    var DestStr = '';
			var ulLength = SrcStr.length;
            var re = /\s/g;
			DestStr = SrcStr.replace(re, '&nbsp;');
			return DestStr;
		},
		TransferHtmlToBlank: function(SrcStr)
		{
		    var DestStr = '';
			var re = /&nbsp;/g;
			DestStr = SrcStr.replace(re, ' ');
			return DestStr;
		},
		AppendOthersFromAjax: function(bfileflag, bfirstAppend, id, AjaxGetStr, AjaxOldStr)
		{
			var NewArray   = AjaxGetStr.split("/|");
			var ulLength   = NewArray.length - 1;
			var OldArray   = AjaxOldStr.split("/|");
			var ulOldLength = NewArray.length - 1;
			var subPath    = '';
			var subOldPath    = '';
                     //var AllPath = ''
			var PathArray;
				
			if (ulLength != ulOldLength)
			{
			    if (false == confirm("Get path error, are you sure to continue?"))
			    {
			        return;
			    }
			}
			id = '#' + id;
			for (var i = 0; i < ulLength; i++)
			{
				var TempArray = NewArray[i].split("/:");
				var TempOldArray = OldArray[i].split("/:");

				//transfer ''-->&nbsp;
				var PathName = $(this).TransferBlanckToHtml(TempArray[0]);
                if (0 == i)
				{
					subPath = PathName;
					subOldPath = TempOldArray[0];
				}
				$(this).BuildcommonAppendStAndAppendOne(TempOldArray[0], PathName, parseInt(TempArray[1]), bfileflag, bfirstAppend, id, i);
			}
			$(':radio').click(function(){path = $(this).findTreePath(); path = path.substring(10);});
			PathArray = path.split("/*");
			if (PathArray.length > 1)
			{
				path = PathArray[0] + '/' + subPath;
				OldPath = PathArray[1] + '-' + subOldPath;
				path = path + "/*" + OldPath;
			}
			else
			{
				path += '/' + subPath + '/*-' + subOldPath;
			}
			//set button enable
			$(this).setButtonEnable();
	    },
		setCurrentRadio: function()
		{
			//set current radio checked
			$(':radio').removeAttr('checked');
			$(this).find('input').attr({checked:'checked'});
		},
		setButtonEnable: function()
		{
			if ($('input:checked').length > 0)
			{
				$(':button').removeAttr('disabled');
			}
			else
			{
				$(':button').eq(1).attr({disabled:'disabled'});
			}
		},
		treeview: function(settings) {
			
			settings = $.extend({
				cookieId: "treeview"
			}, settings);
			
			if (settings.add) {
				return this.trigger("add", [settings.add]);
			}
			
			if ( settings.toggle ) {
				var callback = settings.toggle;
				settings.toggle = function() {
					return callback.apply($(this).parent()[0], arguments);
				};
			}
	
			// handle toggle event
			function toggler() {
				$(this)
					.parent()
					// swap classes for hitarea
					.find(">.hitarea")
						.swapClass( CLASSES.collapsableHitarea, CLASSES.expandableHitarea )
						.swapClass( CLASSES.lastCollapsableHitarea, CLASSES.lastExpandableHitarea )
					.end()
					// swap classes for parent li
					.swapClass( CLASSES.collapsable, CLASSES.expandable )
					.swapClass( CLASSES.lastCollapsable, CLASSES.lastExpandable )
					// find child lists
					.find( ">ul" )
					// toggle them
					.heightToggle( settings.animated, settings.toggle );
				if ( settings.unique ) {
					$(this).parent()
						.siblings()
						// swap classes for hitarea
						.find(">.hitarea")
							.replaceClass( CLASSES.collapsableHitarea, CLASSES.expandableHitarea )
							.replaceClass( CLASSES.lastCollapsableHitarea, CLASSES.lastExpandableHitarea )
						.end()
						.replaceClass( CLASSES.collapsable, CLASSES.expandable )
						.replaceClass( CLASSES.lastCollapsable, CLASSES.lastExpandable )
						.find( ">ul" )
						.heightHide( settings.animated, settings.toggle );
				}
			}
			
			function serialize() {
				function binary(arg) {
					return arg ? 1 : 0;
				}
				var data = [];
				branches.each(function(i, e) {
					data[i] = $(e).is(":has(>ul:visible)") ? 1 : 0;
				});
				$.cookie(settings.cookieId, data.join("") );
			}
			
			function deserialize() {
				var stored = $.cookie(settings.cookieId);
				if ( stored ) {
					var data = stored.split("");
					branches.each(function(i, e) {
						$(e).find(">ul")[ parseInt(data[i]) ? "show" : "hide" ]();
					});
				}
			}
			//hide all ul items
			$(this).find('ul').hide();
			// add treeview class to activate styles
			this.addClass("treeview");
			// prepare branches and find all tree items with child lists
			var branches = this.find("li").prepareBranches(settings, toggler);

			branches.applyClasses(settings, toggler);
				
			// if control option is set, create the treecontroller and show it
			if ( settings.control ) {
				treeController(this, settings.control);
				$(settings.control).show();
			}
			return this.bind("add", function(event, branches) {
				$(branches).prev()
					.removeClass(CLASSES.last)
					.removeClass(CLASSES.lastCollapsable)
					.removeClass(CLASSES.lastExpandable)
				.find(">.hitarea")
					.removeClass(CLASSES.lastCollapsableHitarea)
					.removeClass(CLASSES.lastExpandableHitarea);
			    //hide all ul items
			    $(branches).find('ul').hide();
				$(branches).find('li').andSelf().prepareBranches(settings,toggler).applyClasses(settings, toggler);
			});
		}
	});
	
	// classes used by the plugin
	// need to be styled via external stylesheet, see first example
	var CLASSES = $.fn.treeview.classes = {
		open: "open",
		closed: "closed",
		expandable: "expandable",
		expandableHitarea: "expandable-hitarea",
		lastExpandableHitarea: "lastExpandable-hitarea",
		collapsable: "collapsable",
		collapsableHitarea: "collapsable-hitarea",
		lastCollapsableHitarea: "lastCollapsable-hitarea",
		lastCollapsable: "lastCollapsable",
		lastExpandable: "lastExpandable",
		last: "last",
		hitarea: "hitarea",
		hitareaEx: "hitareaEx"
	};
	
	// provide backwards compability
	$.fn.Treeview = $.fn.treeview;
	
})(jQuery);

PageInfo=function(){
	return{
		top:function(){return document.body.scrollTop||document.documentElement.scrollTop},
		width:function(){return self.innerWidth||document.documentElement.clientWidth},
		height:function(){return self.innerHeight||document.documentElement.clientHeight},
		theight:function(){
			var d=document, b=d.body, e=d.documentElement;
			return Math.max(Math.max(b.scrollHeight,e.scrollHeight),Math.max(b.clientHeight,e.clientHeight))
		},
		twidth:function(){
			var d=document, b=d.body, e=d.documentElement;
			return Math.max(Math.max(b.scrollWidth,e.scrollWidth),Math.max(b.clientWidth,e.clientWidth))
		}
	}
}();
