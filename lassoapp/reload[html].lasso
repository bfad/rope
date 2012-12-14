<!DOCTYPE html>
<html>
<head>
    <title>Rope Controllers Reload List</title>
    <link rel="stylesheet" type="text/css" charset="utf-8" href="[lassoapp_link('/assets/css/styles.css')]" />
</head>
<body class="fixed">
    <h1>Reload Controllers</h1>

    <p>Click on the controller below to reload its file, or <a href="[lassoapp_link('/reload-all')]">reload all controllers</a></p>
    
    <table class="row_highlight">
        <thead>
            <tr><th>Controller Name</th><th>Controller File</th></tr>
        </thead>[local(alt_color) = 0]
    [with row in rope_thread->code_paths->eachPair
     order by #row->first 
     do {^
    ]
        <tr class="alt_color[#alt_color]" onclick="window.location = '[lassoapp_link('/reload-controller')]?c=[#row->first->encodeUrl]'">
            <td>[#row->first]</td>
            <td>[#row->second]</td>
        </tr>[#alt_color = 1 - #alt_color]
    [^}]
    </table>
</body>
</html>