$(function(){
    $("#file_browser")
        .on("click", "li.is_dir" , function(e){ loadListingFor($(this), e) })
        .on("click", "li.is_file", function(e){     reloadFile($(this), e) });
    loadListingFor($("#file_browser"));
});

function loadListingFor(elm, e) {
    if(e)
        e.stopPropagation();

    if(! elm.data('path'))
        return;

    if(elm.hasClass("is_listing")) {
        elm.removeClass('is_listing').children('ul').remove();
        return;
    }

    // Spinner for loading
    elm.addClass("is_listing").append('<div id="circleLoading"><div id="circleLoading_1" class="circleLoading"></div><div id="circleLoading_2" class="circleLoading"></div><div id="circleLoading_3" class="circleLoading"></div><br style="clear: left;" /></div>');
    $.get('list.xhr', {path: elm.data('path'), exts: $("#lasso_exts").val()}, function(data){
        var html = '<ul>';
        for(i in data.files) {
            html += '<li class="is_file" data-path="' + elm.data('path') + '/' + data.files[i] + '">' + data.files[i] + '</li>';
        }
        for(i in data.dirs) {
            html += '<li class="is_dir" data-path="' + elm.data('path') + '/' + data.dirs[i] + '">' + data.dirs[i] + '</li>';
        }
        html += '</ul>';
        elm.children("#circleLoading").remove();
        elm.append(html);
    }, 'json');
}

function reloadFile(elm, e) {
    if(e)
        e.stopPropagation();

    // Hide files and show spinner
    $("#file_browser").hide().after('<div id="circleLoading"><div id="circleLoading_1" class="circleLoading"></div><div id="circleLoading_2" class="circleLoading"></div><div id="circleLoading_3" class="circleLoading"></div><br style="clear: left;" /></div>');
    $.ajax({
             url: 'reload.xhr',
            data: {path: elm.data('path')},
        dataType: 'html', 
         success: function(data) {
            alert("Success reloaded this file.");
            $("#circleLoading").remove();
            $("#file_browser").show();
        },
        error: function(data) {
            alert(data.responseText.replace(/<br \/>\n/g, '\n').replace(/<br \/>/g, '\n').replace(/<\/?[^>]+(>|$)/g, " ").replace(/ ( )+/g, ' '));
            $("#circleLoading").remove();
            $("#file_browser").show();
        }
    });
}

function reloadAll() {
    $("#file_browser").hide().after('<div id="circleLoading"><div id="circleLoading_1" class="circleLoading"></div><div id="circleLoading_2" class="circleLoading"></div><div id="circleLoading_3" class="circleLoading"></div><br style="clear: left;" /></div>');
    $.ajax({
             url: 'reload-all.xhr',
            data: {exts: $("#lasso_exts").val()},
         success: function(data) {
            alert("Successfully reloaded all files.");
            $("#circleLoading").remove();
            $("#file_browser").show();
        },
        error: function(data) {
            alert(data.responseText.replace(/<br \/>\n/g, '\n').replace(/<br \/>/g, '\n').replace(/<\/?[^>]+(>|$)/g, " ").replace(/ ( )+/g, ' '));
            $("#circleLoading").remove();
            $("#file_browser").show();
        }
    });
}
