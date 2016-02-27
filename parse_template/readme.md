A library that accepts a UTF-8 text template and generates text(e.g. html)


Template Tags
--------------
<%="callback_name"%> -	insert text from callback 
<%date%> -	insert current date
<%time%> -	insert current time
<%date_format=""%> -	date format using same format as std::put_time.  Changes the date format on suceeding items
<%time_format=""%> -	date format using same format as std::put_time.  Changes time format on suceeding items
<%repeat="callback_name" prefix="" suffix=""%> -	insert text from callback that generates an iterable list of items.  
													A prefix can be inserted before each item along with a sufix after
													each item.  This can be useful for putting html open/close tags.
													prefix must preceed suffix and if one is specified both must be.
Quotes within parameters can be escaped with a \ (e.g. prefix="<li class=\"list_item\">" suffix="</li>\n")



