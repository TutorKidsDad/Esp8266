<?php
// create a new cURL resource
$ch = curl_init();
//$url = "https://docs.google.com/forms/d/1KeMJCyif0jXXQyLSvTbeMEv9bYQmEizWYG6lImO7OKY/formResponse?ifq&entry.XXX=";
$url = "https://docs.google.com/forms/d/1KeMJCyif0jXXQyLSvTbeMEv9bYQmEizWYG6lImO7OKY/formResponse?ifq&entry.613521783=";
$url .= $_GET ['h'];
$url .= "&entry.1289586739=";
$url .= $_GET ['t'];
// set URL and other appropriate options
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_HEADER, 0);
// grab URL and pass it to the browser
curl_exec($ch);
// close cURL resource, and free up system resources
curl_close($ch);
?>



//https://docs.google.com/forms/d/1KeMJCyif0jXXQyLSvTbeMEv9bYQmEizWYG6lImO7OKY/edit#responses
//http://onlinetps.com/store/XXX.php?h=22&t=33
