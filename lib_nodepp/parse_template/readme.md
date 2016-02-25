A library that accepts a UTF-8 text template and generates text(e.g. html)


Template Tags
--------------
<%="callback_name"%> -	insert text from callback 
<%date%> -	insert current date
<%time%> -	insert current time
<%date_time%> -	insert date time string
<%date_format=""%> -	date format using same format as std::put_time
<%time_format=""%> -	date format using same format as std::put_time
<%repeat="callback_name" style="" alt_style=""%> -	insert text from callback that generates an iterable list of items.  
													The CSS style for each line can be provided, along with an alt_style
													that is for every other line.

Quotes within parameters can be escaped with a \ (e.g. style="image=\"test.jpg\"")



