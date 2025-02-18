BEGIN;


CREATE TABLE IF NOT EXISTS public.users
(
    id_user serial NOT NULL,
    login_user text NOT NULL,
    pass_user text NOT NULL,
    name_user text NOT NULL,
    enable_user boolean NOT NULL DEFAULT true,
    PRIMARY KEY (id_user)
);

COMMENT ON TABLE public.users
    IS 'Users data';

CREATE TABLE IF NOT EXISTS public.all_chat
(
    id_message serial NOT NULL,
    id_user integer NOT NULL,
    message_chatall text NOT NULL,
    deleted_message_chatall boolean NOT NULL,
    PRIMARY KEY (id_message)
);

COMMENT ON TABLE public.all_chat
    IS 'data all''s chat';

CREATE TABLE IF NOT EXISTS public.chat_users
(
    id_chat serial NOT NULL,
    id_useroutput integer NOT NULL,
    id_userinput integer NOT NULL,
    deleted_chat boolean NOT NULL DEFAULT false,
    PRIMARY KEY (id_chat),
    UNIQUE (id_chat)
);

COMMENT ON TABLE public.chat_users
    IS 'data user chat';

CREATE TABLE IF NOT EXISTS public.msg_usr_chat
(
    id_message_chat serial NOT NULL,
    id_chat integer NOT NULL,
    id_user_upload integer NOT NULL,
    message_chat text NOT NULL,
    deleted_message boolean NOT NULL DEFAULT false,
    PRIMARY KEY (id_message_chat),
    UNIQUE (id_message_chat),
    UNIQUE (id_chat)
);

ALTER TABLE IF EXISTS public.all_chat
    ADD FOREIGN KEY (id_user)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.chat_users
    ADD FOREIGN KEY (id_useroutput)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.chat_users
    ADD FOREIGN KEY (id_userinput)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.msg_usr_chat
    ADD FOREIGN KEY (id_chat)
    REFERENCES public.chat_users (id_chat) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;

END;
