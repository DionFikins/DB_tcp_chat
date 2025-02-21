BEGIN;

CREATE TABLE IF NOT EXISTS public.all_chat
(
    id_message serial NOT NULL,
    id_user integer NOT NULL,
    message_chatall text COLLATE pg_catalog."default" NOT NULL,
    deleted_message_chatall boolean NOT NULL DEFAULT false,
    CONSTRAINT all_chat_pkey PRIMARY KEY (id_message)
);

COMMENT ON TABLE public.all_chat
    IS 'data all''s chat';

CREATE TABLE IF NOT EXISTS public.chat_users
(
    id_chat serial NOT NULL,
    id_useroutput integer NOT NULL,
    id_userinput integer NOT NULL,
    deleted_chat boolean NOT NULL DEFAULT false,
    CONSTRAINT chat_users_pkey PRIMARY KEY (id_chat)
);

COMMENT ON TABLE public.chat_users
    IS 'data user chat';

CREATE TABLE IF NOT EXISTS public.msg_usr_chat
(
    id_message_chat serial NOT NULL,
    id_chat integer NOT NULL,
    id_user_upload integer NOT NULL,
    message_chat text COLLATE pg_catalog."default" NOT NULL,
    deleted_message boolean NOT NULL DEFAULT false,
    CONSTRAINT msg_usr_chat_pkey PRIMARY KEY (id_message_chat)
);

CREATE TABLE IF NOT EXISTS public.users
(
    id_user serial NOT NULL,
    login_user text COLLATE pg_catalog."default" NOT NULL,
    pass_user text COLLATE pg_catalog."default" NOT NULL,
    name_user text COLLATE pg_catalog."default" NOT NULL,
    enable_user boolean NOT NULL DEFAULT true,
    CONSTRAINT users_pkey PRIMARY KEY (id_user)
);

COMMENT ON TABLE public.users
    IS 'Users data';

ALTER TABLE IF EXISTS public.all_chat
    ADD CONSTRAINT all_chat_id_user_fkey FOREIGN KEY (id_user)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.chat_users
    ADD CONSTRAINT chat_users_id_userinput_fkey FOREIGN KEY (id_userinput)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.chat_users
    ADD CONSTRAINT chat_users_id_useroutput_fkey FOREIGN KEY (id_useroutput)
    REFERENCES public.users (id_user) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;


ALTER TABLE IF EXISTS public.msg_usr_chat
    ADD CONSTRAINT msg_usr_chat_id_chat_fkey FOREIGN KEY (id_chat)
    REFERENCES public.chat_users (id_chat) MATCH SIMPLE
    ON UPDATE CASCADE
    ON DELETE CASCADE
    NOT VALID;

END;
