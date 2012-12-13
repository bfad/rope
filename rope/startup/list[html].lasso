<!DOCTYPE html>
<html>
<head>
    <title>LassoStartup Files</title>
    <link rel="stylesheet" type="text/css" charset="utf-8" href="[lassoapp_link('/assets/css/styles.css')]" />
    <script type="text/javascript" src="[lassoapp_link('/assets/js/jquery.js')]"></script>
    <script type="text/javascript" src="[lassoapp_link('/assets/js/startup/list.js')]"></script>

    <style type="text/css">
        #file_browser li.is_dir {
            background-image: url("[lassoapp_link('/assets/img/icon_folder.svg')]");
        }
        #file_browser li.is_file {
            background-image: url("[lassoapp_link('/assets/img/icon_lasso_file.svg')]");
        }
    </style>
</head>
<body class="fixed">
    <h1>LassoStartup Files</h1>

    <p>Click on a folder to reveal it's contents. Click on a file to reload it, or <a href="[lassoapp_link('/startup/reload-all')]">reload all files</a></p>
    
    <div id="file_browser" data-path="[sys_userStartupPath]"></div>
</body>
</html>