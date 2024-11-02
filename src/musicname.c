#include <stdio.h>
#include <id3tag.h>

char* convertId3String(const id3_ucs4_t* ucs4){
	if (!ucs4)
		return NULL;

	char* str = (char*)id3_ucs4_utf8duplicate(ucs4);
	return str;
}


char *getName(const char *filename) {
    struct id3_file *file = id3_file_open(filename, ID3_FILE_MODE_READONLY);
    if (file == NULL) {
        printf("ファイルを開けませんでした。\n");
        return NULL;
    }

    struct id3_tag *tag = id3_file_tag(file);
    if (tag == NULL) {
        printf("タグ情報が見つかりませんでした。\n");
        id3_file_close(file);
        return NULL;
    }

    struct {
	    const char* id;
	    const char* name;
    } frames[] = {
	    { ID3_FRAME_TITLE, "name" },
	    { NULL, NULL }
    };

    for(int i = 0; frames[i].id != NULL; i++){
	    struct id3_frame* frame = id3_tag_findframe(tag, frames[i].id, 0);
	    if (frame){
		    const id3_ucs4_t* ucs4 = id3_field_getstrings(&frame->fields[1], 0);
		    char* str = convertId3String(ucs4);

		    if (str){
			    id3_file_close(file);
			    return str;
		    }
	    }
    }

    id3_file_close(file);
    return NULL;
}


