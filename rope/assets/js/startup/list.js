$(function(){
    $("#file_browser").on("click", "li.is_dir", function(e){ loadListingFor($(this)) });
    loadListingFor($("#file_browser"));
});

function loadListingFor(elm) {
    if(! elm.data('path'))
        return;

    // Spinner for loading would be nice

    $.get('list.xhr', {path: elm.data('path')}, function(data){
        var html = '<ul>';
        for(i in data.files) {
            html += '<li class="is_file" data-path="' + elm.data('path') + '/' + data.files[i] + '">' + data.files[i] + '</li>';
        }
        for(i in data.dirs) {
            html += '<li class="is_dir" data-path="' + elm.data('path') + '/' + data.dirs[i] + '">' + data.dirs[i] + '</li>';
        }
        html += '</ul>';
        elm.append(html);
        restripeFileBrowser();
    }, 'json');
}

function restripeFileBrowser() {
    $("#file_browser li").removeClass('alt_color0').removeClass('alt_color1');

    $("#file_browser li:even").addClass('alt_color0');
    $("#file_browser li:odd") .addClass('alt_color1');
}