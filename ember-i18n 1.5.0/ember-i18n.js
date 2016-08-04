(function() {
  var I18n, findTemplate, getPath, isBinding, isTranslatedAttribute, pluralForm;

  isTranslatedAttribute = /(.+)Translation$/;

  getPath = Ember.Handlebars.get || Ember.get;

  if (typeof CLDR !== "undefined" && CLDR !== null) pluralForm = CLDR.pluralForm;

  if (pluralForm == null) {
    Ember.Logger.warn("CLDR.pluralForm not found. Em.I18n will not support count-based inflection.");
  }

  findTemplate = function(key, context, setOnMissing) {
    var result;
    Ember.assert("You must provide a translation key string, not %@".fmt(key), typeof key === 'string');
    result = I18n.translations[key];
    //Em.Logger.log("\n\n====[" + key + "]\n" + result);

    findEmbededAttrs = function(keyStr, attrs) {
    	var parts = keyStr.split("{{");
    	if (0 == parts.length) {
    		return [keyStr, false];
    	}
    	var newKeyStr = "";
    	var canCache = true;
    	parts.forEach(function (part) {
    		var endPos = part.indexOf("}}");
    		if (-1 == endPos) {
    			newKeyStr += part;
    		} else {
    			newKeyStr += "{{";
    			var newKey = part.substring(0, endPos);
    			if (-1 != newKey.indexOf(".")) {
    				var validKey = newKey.replace(/\./g, '_');
    				attrs[validKey] = Em.I18n.t(newKey);
    				newKeyStr += validKey;
    				canCache = false;
    				newKeyStr += part.substring(endPos);
    			} else {
    				newKeyStr += part;
    			}
    		}
    	});
    	return [newKeyStr, canCache];
    };
    var canCache = true;
    if ("string" == typeof result) {
    	var tmpRes = findEmbededAttrs(result, context);
    	result = tmpRes[0];
    	canCache = tmpRes[1];
    }

    if (setOnMissing) {
      if (result == null) {
        result = I18n.compile("Missing translation: " + key);
      }
    }
    if ((result != null) && !$.isFunction(result)) {
      result = I18n.compile(result);
    }
    if ((null != result) && canCache) {
    	I18n.translations[key] = result;
    }
    return result;
  };

  I18n = {
    compile: Handlebars.compile,
    translations: {},
    addTranslations: function (input) {
      for (var key in input)
      {
        this.translations[key] = input[key];
      }
    },
    template: function(key, context, count) {
      var interpolatedKey, result, suffix;
      if ((count != null) && (pluralForm != null)) {
        suffix = pluralForm(count);
        interpolatedKey = "%@.%@".fmt(key, suffix);
        result = findTemplate(interpolatedKey, context, false);
      }
      return result != null ? result : findTemplate(key, context, true);
    },
    t: function(key, context) {
      var template;
      if (context == null) context = {};
      template = I18n.template(key, context, context.count);
      return template(context);
    },
    TranslateableAttributes: Em.Mixin.create({
      didInsertElement: function() {
        var attribute, isTranslatedAttributeMatch, key, path, result, translatedValue;
        result = this._super.apply(this, arguments);
        for (key in this) {
          path = this[key];
          isTranslatedAttributeMatch = key.match(isTranslatedAttribute);
          if (isTranslatedAttributeMatch) {
            attribute = isTranslatedAttributeMatch[1];
            translatedValue = I18n.t(path);
            this.$().attr(attribute, translatedValue);
          }
        }
        return result;
      }
    })
  };

  Em.I18n = I18n;

  Ember.I18n = I18n;

  isBinding = /(.+)Binding$/;

  Handlebars.registerHelper('t', function(key, options) {
    var attrs, context, elementID, result, tagName, view;
    context = this;
    attrs = options.hash;
    view = options.data.view;
    tagName = attrs.tagName || 'span';
    delete attrs.tagName;
    elementID = "i18n-" + (jQuery.uuid++);
    Em.keys(attrs).forEach(function(property) {
      var bindPath, currentValue, invoker, isBindingMatch, observer, propertyName;
      isBindingMatch = property.match(isBinding);
      if (isBindingMatch) {
        propertyName = isBindingMatch[1];
        bindPath = attrs[property];
        currentValue = getPath(context, bindPath);
        attrs[propertyName] = currentValue;
        invoker = null;
        observer = function() {
          var elem, newValue;
          newValue = getPath(context, bindPath);
          elem = view.$("#" + elementID);
          if (elem.length === 0) {
            Em.removeObserver(context, bindPath, invoker);
            return;
          }
          attrs[propertyName] = newValue;
          return elem.html(I18n.t(key, attrs));
        };
        invoker = function() {
          return Em.run.once(observer);
        };
        return Em.addObserver(context, bindPath, invoker);
      }
    });
	if ((-1 == key.indexOf(".")) || (0 == key.indexOf("this.")) || (0 == key.indexOf("view."))) {
		if (getPath(view, key)) {
	    	//Em.Logger.log("1 +===============" + key);
	      key = getPath(view, key);
	    }
	    else if (getPath(this, key)){
	    	//Em.Logger.log("2 +===============" + key);
	      key = getPath(this, key);
	    }
	    else if (getPath(view.get("parentView"), key)){
	    	//Em.Logger.log("3 +===============" + key);
	      key = getPath(view.get("parentView"), key);
	    } else {
	    	//Em.Logger.log("4 +===============" + key);
	    }
	} else {
		//Em.Logger.log("5 +===============" + key);
	}
    result = '<%@ id="%@">%@</%@>'.fmt(tagName, elementID, I18n.t(key, attrs), tagName);
    return new Handlebars.SafeString(result);
  });

  Handlebars.registerHelper('translateAttr', function(options) {
    var attrs, result;
    attrs = options.hash;
    result = [];
    Em.keys(attrs).forEach(function(property) {
      var translatedValue;
      translatedValue = I18n.t(attrs[property]);
      return result.push('%@="%@"'.fmt(property, translatedValue));
    });
    return new Handlebars.SafeString(result.join(' '));
  });

}).call(this);
